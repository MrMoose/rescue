// Copyright 2018 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "InputParser.hpp"

#include "tools/Log.hpp"
#include "tools/Assert.hpp"
#include "tools/Error.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/phoenix.hpp>
#include <boost/variant.hpp>

#include <bitset>
#include <locale>
#include <list>

BOOST_FUSION_ADAPT_STRUCT(
    moose::rescue::string_var,
    (std::vector<std::string>, m_options))

namespace moose {
namespace rescue {

using namespace tools;
namespace qi = boost::spirit::qi;
namespace ascii = qi::ascii;
namespace phx = boost::phoenix;

template <typename Iterator>
struct text_parser : qi::grammar<Iterator, std::string()> {

	text_parser() : text_parser::base_type{start} {

		start     %= +((ascii::char_ | qi::punct) - ascii::char_("[] "));
	}

	qi::rule<Iterator, std::string()> start;
};

template <typename Iterator>
struct whitespace_parser : qi::grammar<Iterator, moose::rescue::whitespace()> {

	whitespace_parser() : whitespace_parser::base_type{start} {

		start = qi::char_(' ')[qi::_val = phx::construct<moose::rescue::whitespace>()];
	}

	qi::rule<Iterator, moose::rescue::whitespace()> start;
};

template <typename Iterator>
struct variant_parser : qi::grammar<Iterator, moose::rescue::string_var()> {

	variant_parser() : variant_parser::base_type{start} {

		option %= +((ascii::char_ | qi::punct) - ascii::char_("[]| "));

		start %= '[' >> (option % '|') >> ']';
	}


	qi::rule<Iterator, std::string()>               option;
	qi::rule<Iterator, moose::rescue::string_var()> start;
};

template <typename Iterator>
struct input_parser : qi::grammar<Iterator, std::list<moose::rescue::token>()> {

	input_parser() : input_parser::base_type{start} {

		token %= whitespace | str_var | text;
		start %= *token;
	}

	text_parser<Iterator>                                 text;
	whitespace_parser<Iterator>                           whitespace;
	variant_parser<Iterator>                              str_var;
	qi::rule<Iterator, moose::rescue::token()>            token;
	qi::rule<Iterator, std::list<moose::rescue::token>()> start;
};

std::list<token> RESCUE_API parse_input_string(const std::string &n_input) {

	std::list<token> ret;

	std::string::const_iterator begin = n_input.begin();
	const std::string::const_iterator end = n_input.end();
	input_parser<std::string::const_iterator> p;

	if (!qi::parse(begin, end, p, ret) || (begin != end)) {
		BOOST_THROW_EXCEPTION(serialization_error() << error_message("Cannot parse input string") << error_argument(n_input));
	} else {
		return ret;
	}
}

} // namespace rescue
} // namespace moose
