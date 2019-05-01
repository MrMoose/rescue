// Copyright 2019 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "WorkQueue.hpp"
#include "XorEnc.hpp"
#include "Sha512.hpp"

#include "mredis/AsyncClient.hpp"
#include "mredis/BlockingRetriever.hpp"

#include "tools/Log.hpp"
#include "tools/Assert.hpp"
#include "tools/Error.hpp"

#include <boost/variant.hpp>

#include <locale>
#include <list>


/*
	Work Q consistes of the following:

	KEY                 VALUE                         EXPLANATION
  ----------------------------------------------------------------------------------------------
	rsc:passwords       hash of string to string      possible candidates for password
													  key is the hash and value is the xor encrypted password

	rsc:candidates      set of strings                set of hashes, which are keys to
													  passwords that have not been processed yet.
													  When one is processed, it is removed from here
													  and put into failed (or success ;-)

	rsc:lease:$HASH     string                        value is the xor encrypted password. Expire time is one minute.
													  As long as this exists, the candidate is considered locked

	rsc:failed          set of strings                failed attempts as hash

	rsc:success         set of strings                successful attempt(s?) as hash

 */



namespace moose {
namespace rescue {

using namespace moose::tools;

/* Add the entry if not already done so
 *
 * KEYS[1]      rsc:lease:$HASH
 *
 * ARGV[1]      password
 * ARGV[2]      hash
 *
 * returns:
 *     0 on success
 *    -1 on password already known
 */
const std::string enter_candidate{

	// If a lease already exists, the candidate and everything else must exist as well.
	// this means the passwd is already known. Cheap check early
	"if redis.call('exists', KEYS[1]) == 1 then "
	    "return -1 "
	"end "

	// The same is true is the entry is already added. Expensive check later
	"if redis.call('hexists', 'rsc:passwords', ARGV[2]) == 1 then "
	    "return -1 "
	"end "

	// If we are here, we can safely insert the value. Into the hash -> value map first
	"redis.call('hset', 'rsc:passwords', ARGV[2], ARGV[1]) "

	// and into the set of candidates
	"redis.call('sadd', 'rsc:candidates', ARGV[2]) "
	"return 0"
};

bool queue_candidate(mredis::AsyncClientSPtr n_redis, const std::string &n_candidate) {

	MOOSE_ASSERT(n_redis)

	if (n_candidate.empty()) {
		BOOST_LOG_SEV(logger(), warning) << "Empty candidate could not be queued";
		return false;
	}

	try {
		const std::string hash = sha512(n_candidate);

		const std::vector<std::string> keys{
			"rsc:lease:" + hash
		};

		const std::vector<std::string> args{
			encrypt_decrypt(n_candidate),
			hash
		};

		BOOST_LOG_SEV(logger(), normal) << "Inserting hash for password candidate: " << hash;

		mredis::BlockingRetriever<boost::int64_t> inserter{ 15 };
		n_redis->eval(enter_candidate, keys, args, inserter.responder());
		const boost::optional<boost::int64_t> result = inserter.wait_for_response();

		if (!result) {
			BOOST_THROW_EXCEPTION(internal_error() << error_message("No response from insertion script"));
		} else {
			switch (*result) {
				case 0:
					BOOST_LOG_SEV(logger(), normal) << "Candidate inserted successfully";
					break;
				case -1:
					BOOST_LOG_SEV(logger(), warning) << "Candidate not inserted, already present";
					return false;
				default:
					BOOST_THROW_EXCEPTION(internal_error()
					            << error_message("Unexpected result from insertion script")
					            << error_argument(*result));
			}
		}

		return true;

	} catch (const moose_error &merr) {
		BOOST_LOG_SEV(logger(), error) << "Failed to insert password candidate:"
		        << boost::diagnostic_information(merr);
	}

	return false;
}

/* Poll an entry from the work queue
 *
 * returns:
 *     xor encrypted password when lease is established (for 1 minute)
 *     nil when no work item is available
 */
const std::string poll_queue{

	// Iterate the set of candidates until we either find one that doesn't have a lease
	// or end of the set
	"local keys = {} "
	"local done = false "
	"local cursor = '0' "
	"repeat "
	    "local result = redis.call('sscan', cursor, 'rsc:candidates') "
	    "cursor = result[1] "
	    "keys = result[2] "
	    "for i, hash in ipairs(keys) do "
	        "local lease_key = 'rsc:lease:' .. hash "

	        "if redis.call('exists', lease_key) == 0 then "

	            // This is the candidate we are looking for. Get the password associated with
	            // it and create a lease

	            "local enc_password = redis.call('get', 'rsc:passwords', hash) "
	            "redis.call('set', lease_key, enc_password, 'EX', '60) "
	            "return enc_password "
	        "end "
	    "end "
	    "if cursor == '0' then "
	        "done = true "
	    "end "
	"until done "
	"return nil "
};

bool poll_candidate(mredis::AsyncClientSPtr n_redis, std::string &n_candidate) {

	MOOSE_ASSERT(n_redis)

	try {
		const std::vector<std::string> keys;
		const std::vector<std::string> args;

		BOOST_LOG_SEV(logger(), normal) << "Polling work queue for password candidate...";

		mredis::BlockingRetriever<std::string> poller{ 15 };
		n_redis->eval(poll_queue, keys, args, poller.responder());
		const boost::optional<std::string> result = poller.wait_for_response();

		if (!result || result->empty()) {
			// we have no item in the Q that isn't already leased
			return false;
		} else {
			// Decrypt and return. Caller now has 60 seconds to work on it.
			n_candidate = encrypt_decrypt(*result);
			return true;
		}

	} catch (const moose_error &merr) {
		BOOST_LOG_SEV(logger(), error) << "Failed to to poll password candidate: "
		        << boost::diagnostic_information(merr);
	}

	return false;
}

/* Return a lease and remove the item from the work queue
 *
 * KEYS[1]      rsc:lease:$HASH
 *
 * ARGV[1]      hash
 * ARGV[2]      "0" on no success, "1" on success
 *
 * returns:
 *     0 on everything OK
 *    -1 when item was not leased (or lease expired)
 */
const std::string return_lease{

	// If a lease doesn't exist, I will return false but still mark success
	"if redis.call('exists', KEYS[1]) == 0 then "
	    "if ARGV[2] == '1' then "
	        "redis.call('sadd', 'rsc:success', ARGV[1]) "
	    "endif "
	    "return -1 "
	"end "

	// Otherwise remove the lease and the candidates entry
	"redis.call('del', KEYS[1]) "
	"redis.call('srem', 'rsc:candidates', ARGV[1]) "

	"if ARGV[2] == '1' then "
	    "redis.call('sadd', 'rsc:success', ARGV[1]) "    // YYYYYEEEEEAAAAAAAHHHHHHH!!!!!
	"else "
	    "redis.call('sadd', 'rsc:failed', ARGV[1]) "
	"endif "
	"return 0 "
};

void return_candidate(mredis::AsyncClientSPtr n_redis, std::string &n_candidate, const bool n_success) {

	MOOSE_ASSERT(n_redis)

	try {
		const std::string hash = sha512(n_candidate);

		const std::vector<std::string> keys {
			"rsc:lease:" + hash
		};

		const std::vector<std::string> args {
			hash,
			n_success ? "1" : "0"
		};

		BOOST_LOG_SEV(logger(), normal) << "Polling work queue for password candidate...";

		mredis::BlockingRetriever<boost::int64_t> returner{ 15 };
		n_redis->eval(return_lease, keys, args, returner.responder());
		const boost::optional<boost::int64_t> result = returner.wait_for_response();

		if (!result) {
			// we have no item in the Q that isn't already leased
			BOOST_THROW_EXCEPTION(internal_error() << error_message("No response from return lease script"));
		} else {
			if (*result != 0) {
				BOOST_LOG_SEV(logger(), warning) << "Return lease script returned " << *result << ". Maybe you should check that out";
			}
		}

	} catch (const moose_error &merr) {
		BOOST_LOG_SEV(logger(), error) << "Failed to return lease: "
		        << boost::diagnostic_information(merr);
	}
}



} // namespace rescue
} // namespace moose
