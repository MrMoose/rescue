
//  Copyright 2015 Stephan Menzel. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE InputTests
#include <boost/test/unit_test.hpp>

#include "rescue/InputGenerator.hpp"
#include "tools/Error.hpp"

#include <boost/algorithm/string.hpp>

#include <set>
#include <iostream>
#include <vector>
#include <string>

using namespace moose::tools;
using namespace moose::rescue;

#if defined RESCUE_ALNUM_TOKEN_PERMUTATIONS_ALL_LETTERS

BOOST_AUTO_TEST_CASE(OneStringPerms) {

	const std::string input("hEl");

	std::vector<std::string> perms;

	BOOST_REQUIRE_NO_THROW(case_permutations(input, perms));

	BOOST_CHECK(perms.size() == std::pow(2, input.size()));

	BOOST_CHECK(perms[0] == "hel");
	BOOST_CHECK(perms[1] == "Hel");
	BOOST_CHECK(perms[2] == "hEl");
	BOOST_CHECK(perms[3] == "HEl");
	BOOST_CHECK(perms[4] == "heL");
	BOOST_CHECK(perms[5] == "HeL");
	BOOST_CHECK(perms[6] == "hEL");
	BOOST_CHECK(perms[7] == "HEL");
}

#elif defined RESCUE_ALNUM_TOKEN_PERMUTATIONS_FIRST_LETTER_ONLY

BOOST_AUTO_TEST_CASE(OneStringPerms) {

	const std::string input("hEl");

	std::vector<std::string> perms;

	BOOST_REQUIRE_NO_THROW(case_permutations(input, perms));

	BOOST_CHECK(perms.size() == 2);

	BOOST_CHECK(perms[0] == "HEl");
	BOOST_CHECK(perms[1] == "hEl");
}

#else
    #pragma message ("Alnum token behavior undefined.")
#endif


BOOST_AUTO_TEST_CASE(Diverse) {

	const std::string input("Hi [wo|rl]!");

	std::vector<std::string> perms;

	BOOST_REQUIRE_NO_THROW(generate_permutations(input, perms));

	// Well, I don't know how I could simulate an expected result.
	// by looking at it and considering it reasonable I
	// think it should suffice to check for 384 items
#if defined RESCUE_ALNUM_TOKEN_PERMUTATIONS_ALL_LETTERS

	BOOST_CHECK(perms.size() == 384);

#elif defined RESCUE_ALNUM_TOKEN_PERMUTATIONS_FIRST_LETTER_ONLY

	BOOST_CHECK(perms.size() == 96);

#endif
}
