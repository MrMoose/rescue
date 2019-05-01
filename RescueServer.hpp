// Copyright 2019 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "RescueConfig.hpp"

#include "mredis/FwdDeclarations.hpp"

#include "tools/Macros.hpp"

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include <memory>
#include <string>

namespace moose {
namespace rescue {

/*! @brief Primary rescue server component.
 * Spawns as many threads as cores are available and has them poll
 * the work queue and work upon extracted items
 */
class RescueServer final {

	public:
		/*! @brief create the server object

		 */
		RESCUE_API RescueServer(const std::string &n_redis_server);

		//! shut down everything
		RESCUE_API ~RescueServer() noexcept;
		
		//! start server and block until signal is received
		RESCUE_API void run(const boost::filesystem::path &n_luks_file);

		//! shut down anyway
		RESCUE_API void shutdown();

	private:
		boost::thread_group            m_workers;
		moose::mredis::AsyncClientSPtr m_redis;    // globally used by all workers
};

} // namespace rescue
} // namespace moose
