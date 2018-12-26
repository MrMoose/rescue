// Copyright 2018 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "RescueConfig.hpp"

#include <vector>
#include <string>
#include <list>

namespace moose {
namespace rescue {

/*!
 * @brief generate all possible permutations out of an input string
 *
 * Text stays text but is generated with all permutations of upper and lower case.
 * Whitespace becomes:
 *	 * space
 *	 * tab
 *	 * underscore
 *	 * dot
 *	 * dash
 *	 * nothing
 * Alternatives are [something|another] where also lower and upper case is permutated.
 *
 * @param n_input as described
 * @param n_output
 * @return the number of generated values
 *
 * @throw serialization_error
 */
std::size_t RESCUE_API generate_permutations(const std::string &n_input, std::vector<std::string> &n_output);

/* Exposed for unit tests */
void RESCUE_API case_permutations(const std::string &n_input, std::vector<std::string> &n_output);


} // namespace rescue
} // namespace moose
