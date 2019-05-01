// Copyright 2018 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "RescueConfig.hpp"
#include "Types.hpp"

#include "mredis/FwdDeclarations.hpp"

#include <vector>
#include <string>
#include <list>

namespace moose {
namespace rescue {

/*! @brief enter a candidate into the work queue on redis

	@param n_input as described
	@return true when entered, false when it was detected as already tried or already queued

	@throw redis_error
 */
bool RESCUE_API queue_candidate(mredis::AsyncClientSPtr n_redis, const std::string &n_candidate);

/*! @brief poll the work queue for candidates

	@return true on candidate available and lease established otherwise false
 */
bool RESCUE_API poll_candidate(mredis::AsyncClientSPtr n_redis, std::string &n_candidate);

/*! @brief return a lease to the work queue with results
	@param n_candidate the clear text candidate to return
	@param n_success set to true when the actually worked out (lucky you!), otherwise false

	@return true on candidate available and lease established otherwise false
 */
void RESCUE_API return_candidate(mredis::AsyncClientSPtr n_redis, std::string &n_candidate, const bool n_success);

} // namespace rescue
} // namespace moose
