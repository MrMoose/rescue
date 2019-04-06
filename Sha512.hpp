// Copyright 2018 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "RescueConfig.hpp"

#include <string>

namespace moose {
namespace rescue {

RESCUE_API std::string sha512(const std::string &n_input);

} // namespace rescue
} // namespace moose
