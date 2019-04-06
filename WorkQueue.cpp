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

	rsc:candidates      list of strings               list of hashes, which are keys to
													  passwords that have not been processed yet.
													  When one is processed, it is removed from here
													  and put into failed (or success ;-)

	rsc:lease:$HASH     string                        value is the xor encrypted password. Expire time is one minute.
													  As long as this exists, the candidate is considered locked

	rsc:failed          list of strings               failed attempts as hash

	rsc:success                                       successful attempt(s?)

 */



namespace moose {
namespace rescue {

using namespace moose::tools;
using namespace moose::mredis;

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

	// If the lease already exists, the candidate and everything else must exist as well.
	// this means the passwd is already known. Cheap check early
	"if redis.call('exists', KEYS[1]) == 1 then "
	    "return -1 "
	"end "

	// The same is true is the entry is already added. Expensive check later
	"if redis.call('hexists', 'rsc:passwords', ARGV[2]) == 1 then "
	    "return -1 "
	"end "

	// If we are here, we can safely insert the value
	"redis.call('hset', 'rsc:passwords', ARGV[2], ARGV[1]) "
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
			"rs::lease:" + hash
		};

		const std::vector<std::string> args{
			encrypt_decrypt(n_candidate),
			hash
		};

		BOOST_LOG_SEV(logger(), normal) << "Inserting hash for password candidate: " << hash;

		BlockingRetriever<boost::int64_t> inserter{ 5 };
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







} // namespace rescue
} // namespace moose
