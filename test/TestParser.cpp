
//  Copyright 2015 Stephan Menzel. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE InputTests
#include <boost/test/unit_test.hpp>

#include "rescue/InputParser.hpp"
#include "tools/Error.hpp"

#include <boost/algorithm/string.hpp>

#include <set>
#include <iostream>

using namespace moose::tools;
using namespace moose::rescue;

bool is_string(const token &n_candidate) {

	return n_candidate.which() == 0;
}

bool is_whitespace(const token &n_candidate) {

	return n_candidate.which() == 1;
}

bool is_option(const token &n_candidate) {

	return n_candidate.which() == 2;
}

BOOST_AUTO_TEST_CASE(FourTokens) {

	const std::string input("Hi [wo|rl]!");

	std::list<token> tokens;

	BOOST_REQUIRE_NO_THROW(tokens = parse_input_string(input));

	BOOST_REQUIRE(tokens.size() == 4);

	for (unsigned int i = 0; i < 4; i++) {

		switch(i) {
			case 0:
				BOOST_CHECK(is_string(tokens.front()));
				break;
			case 1:
				BOOST_CHECK(is_whitespace(tokens.front()));
				break;
			case 2:
				BOOST_CHECK(is_option(tokens.front()));
				break;
			case 3:
				BOOST_CHECK(is_string(tokens.front()));
				break;
		}

		tokens.pop_front();
	}
}
