// Copyright 2019 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "RescueConfig.hpp"

#include <string>

namespace moose {
namespace rescue {

/*! @brief Simple XOR symmetric encryption
	Rescue assumes the redis DB is under your control and
	that you are OK with storing the passwd candidates in there.
	Still, a very simple XOR enc will them render not human readable
	just in case.
 */
RESCUE_API std::string encrypt_decrypt(const std::string &n_source);

} // namespace rescue
} // namespace moose
