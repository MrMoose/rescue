
//  Copyright 2015 Stephan Menzel. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE Sha512Tests
#include <boost/test/unit_test.hpp>

#include "rescue/Sha512.hpp"
#include "tools/Error.hpp"

#include <boost/algorithm/string.hpp>

#include <set>
#include <string>

using namespace moose::tools;
using namespace moose::rescue;

BOOST_AUTO_TEST_CASE(Sha512) {


	std::string src("grape");

	std::string hash;

	BOOST_REQUIRE_NO_THROW(hash = sha512(src));
	BOOST_CHECK(hash == "9375d1abdb644a01955bccad12e2f5c2bd8a3e226187e548d99c559a99461453b980123746753d07c169c22a5d9cc75cb158f0e8d8c0e713559775b5e1391fc4");

}
