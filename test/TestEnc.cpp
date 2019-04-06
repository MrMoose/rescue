
//  Copyright 2015 Stephan Menzel. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE EncryptionTests
#include <boost/test/unit_test.hpp>

#include "rescue/XorEnc.hpp"
#include "tools/Error.hpp"

#include <boost/algorithm/string.hpp>

#include <set>
#include <string>

using namespace moose::tools;
using namespace moose::rescue;

BOOST_AUTO_TEST_CASE(EncDec) {


	std::string src("Hello World!");

	std::string enc;

	BOOST_REQUIRE_NO_THROW(enc = encrypt_decrypt(src));
	BOOST_CHECK(enc != src);

	std::string dec;
	BOOST_REQUIRE_NO_THROW(dec = encrypt_decrypt(enc));
	BOOST_CHECK(dec == src);
}
