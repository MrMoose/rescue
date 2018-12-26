// Copyright 2018 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "RescueServer.hpp"

#include "mredis/AsyncClient.hpp"

#include "tools/Log.hpp"
#include "tools/Assert.hpp"
#include "tools/Error.hpp"

#include <boost/cstdint.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include <exception>
#include <chrono>

namespace moose {
namespace rescue {

using namespace moose::tools;
namespace fs = boost::filesystem;
using std::chrono::seconds;

RescueServer::RescueServer()
		: m_io_context()
		, m_signals(m_io_context) {

	m_signals.add(SIGINT);
	m_signals.add(SIGTERM);
#if defined(SIGQUIT)
	m_signals.add(SIGQUIT);
#endif

	BOOST_LOG_SEV(logger(), normal) << "Rescue Server instance created";
}

RescueServer::~RescueServer() noexcept {

	BOOST_LOG_SEV(logger(), normal) << "destroying Rescue Server instance";
}

void RescueServer::run() {

	BOOST_LOG_NAMED_SCOPE("run");
	try {
		BOOST_LOG_SEV(logger(), normal) << "Stage Server starting up";

		// First I register this as signal handler
		// This thread will pretty much go to sleep and wait for signals
		// once the actual services are running
		m_signals.async_wait(
			[this](boost::system::error_code /*ec*/, int n_signo) {

			BOOST_LOG_SEV(logger(), normal) << "Signal " << n_signo << " received, shutting down";
			shutdown();
		});

		// this thread will run io_context and effectively sleep until signal
		m_io_context.run();
	
	} catch (const moose_error &merr) {
		BOOST_LOG_SEV(logger(), error) << "Stage Server terminated with an error: " << boost::diagnostic_information(merr);
	} catch (const std::exception &sex) {
		BOOST_LOG_SEV(logger(), error) << "Unexpected exception running Stage Server: " << boost::diagnostic_information(sex);
	}

	return;
}

void RescueServer::shutdown() {

	m_io_context.stop();

}

} // namespace rescue
} // namespace moose
