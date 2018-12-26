// Copyright 2018 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "RescueConfig.hpp"

#include "mredis/FwdDeclarations.hpp"

#include "tools/Macros.hpp"

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <memory>

namespace moose {
namespace rescue {

//! @brief Primary stage server component.
//! 
class RescueServer final {

	public:
		/*! @brief create the server object

		 */
		RESCUE_API RescueServer();

		//! shut down everything
		RESCUE_API ~RescueServer() noexcept;
		
		//! start server and block until signal is received
		RESCUE_API void run();

		//! shut down anyway
		RESCUE_API void shutdown();

	private:

		/// See if some asio can be used to keep the server alive until signal
		boost::asio::io_context     m_io_context;          //!< runs pretty much everything. Implicit strand, do not add threads!
		boost::asio::signal_set     m_signals;

		moose::mredis::AsyncClientSPtr m_redis;            //!< only set in cloud environment
};

} // namespace rescue
} // namespace moose
