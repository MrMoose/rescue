// Copyright 2019 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "RescueServer.hpp"
#include "WorkQueue.hpp"
#include "BruteForceLuks.hpp"

#include "mredis/AsyncClient.hpp"

#include "tools/Log.hpp"
#include "tools/Assert.hpp"
#include "tools/Error.hpp"

#include <boost/cstdint.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>

#include <exception>
#include <iostream>
#include <atomic>
#include <chrono>

namespace moose {
namespace rescue {

using namespace moose::tools;
namespace fs = boost::filesystem;
using std::chrono::seconds;

RescueServer::RescueServer(const std::string &n_redis_server)
        : m_redis{ std::make_shared<mredis::AsyncClient>(n_redis_server) } {

	BOOST_LOG_SEV(logger(), normal) << "Rescue Server instance created";
}

RescueServer::~RescueServer() noexcept {

	BOOST_LOG_SEV(logger(), normal) << "destroying Rescue Server instance";
}


void worker(mredis::AsyncClientSPtr n_redis, std::atomic<bool> &n_continue, const fs::path &n_path) {

	try {

		while (n_continue.load()) {

			boost::this_thread::sleep_for(boost::chrono::seconds(1));

			std::string password_candidate;
			if (!poll_candidate(n_redis, password_candidate)) {
				BOOST_LOG_SEV(logger(), debug) << "No password candidates ready. Put some in! Gimme work!";
				continue;
			}

			BOOST_LOG_SEV(logger(), normal) << "Polled password candidate, ready to work";
			bool result = attempt_password(n_path, password_candidate);

			return_candidate(n_redis, password_candidate, result);

			if (result) {
				// Make some noise!
				BOOST_LOG_SEV(logger(), normal) << "YOU HAVE DONE IT!!: " << password_candidate;
				std::cout << "YOU HAVE DONE IT!!: " << password_candidate << std::endl;
				n_continue.store(false);
			}
		}

	} catch (const moose_error &merr) {


	} catch (const std::exception &sex) {


	} catch (...) {


	}


}



void RescueServer::run(const boost::filesystem::path &n_luks_file) {

	BOOST_LOG_NAMED_SCOPE("run")
	try {
		BOOST_LOG_SEV(logger(), normal) << "Rescue Server starting up";

		m_redis->connect();

		std::atomic<bool> go_on{ true };

		// Add as many workers as we have cores and start crunching
		for (unsigned int i = 0; i < boost::thread::hardware_concurrency(); i++) {
			m_workers.add_thread(new boost::thread{[&] { worker(this->m_redis, go_on, n_luks_file); }});
		}

		m_workers.join_all();

	} catch (const moose_error &merr) {
		BOOST_LOG_SEV(logger(), error) << "Stage Server terminated with an error: " << boost::diagnostic_information(merr);
	} catch (const std::exception &sex) {
		BOOST_LOG_SEV(logger(), error) << "Unexpected exception running Stage Server: " << boost::diagnostic_information(sex);
	}

	return;
}

void RescueServer::shutdown() {

}

} // namespace rescue
} // namespace moose
