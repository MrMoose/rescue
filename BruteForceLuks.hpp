// Copyright 2018 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "RescueConfig.hpp"
#include "Types.hpp"

#include "mredis/FwdDeclarations.hpp"

#include <boost/filesystem.hpp>

#include <vector>
#include <string>
#include <list>

namespace moose {
namespace rescue {

/*! @brief brute force attempt to open a Luks volume.
 */
bool attempt_password(const boost::filesystem::path &n_header_file, const std::string &n_password);

} // namespace rescue
} // namespace moose
