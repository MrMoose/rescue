// Copyright 2018 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "RescueConfig.hpp"

#include <boost/variant.hpp>

#include <vector>
#include <string>

namespace moose {
namespace rescue {

/*! @brief A whitespace character
	Input string contained a single ' '

	permutations yield:
	 * space
	 * tab
	 * underscore
	 * dot
	 * dash
	 * nothing
 */
struct whitespace {};

/*! @brief Variant of strings
	Input string contained '[something|another|even_more]'

	permutations yield:
	 * something
	 * another
	 * even_more
 */
struct string_var {
    std::vector<std::string> m_options;
};

/*! @brief Raw Text
	Input string contained 'something'
	Input chars: ASCII + punct

	permutations yield:
	 * Something, sOmething and all other permutations of upper and lower case

    Original casing is discarded
 */

using token = boost::variant<
    std::string,
    whitespace,
    string_var
>;


} // namespace rescue
} // namespace moose
