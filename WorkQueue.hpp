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

/*!
 * @brief enter a candidate into the work queue on redis
 *
 * @param n_input as described
 * @return true when entered, false when it was detected as already tried or already queued
 *
 * @throw redis_error
 */
bool RESCUE_API queue_candidate(mredis::AsyncClientSPtr n_redis, const std::string &n_candidate);

} // namespace rescue
} // namespace moose
