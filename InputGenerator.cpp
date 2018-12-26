// Copyright 2018 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "InputGenerator.hpp"
#include "InputParser.hpp"

#include "tools/Log.hpp"
#include "tools/Assert.hpp"
#include "tools/Error.hpp"

#include <boost/variant.hpp>

#include <bitset>
#include <locale>
#include <list>

namespace moose {
namespace rescue {


/* create all upper and lowercase permutations of a string */
void case_permutations(const std::string &n_input, std::vector<std::string> &n_output) {

	std::locale loc;

	// Now we're gonna create a lot of strings
	const std::uint64_t permutations = std::pow(2, n_input.size());

	for (unsigned int i = 0; i < permutations; i++) {

		std::string permutation = n_input;

		// I'm going to go bitwise over i and uppercase 1 and lowercase 0
		// Using the counter as a bitset to make sure I reflect all permutations
		std::bitset<64> bitset{i} ;
		for (unsigned int num_char = 0; num_char < n_input.size(); num_char++) {
			if (bitset[num_char] == 0) {
				permutation[num_char] = std::tolower(permutation[num_char], loc);
			} else {
				permutation[num_char] = std::toupper(permutation[num_char], loc);
			}
		}

		n_output.push_back(permutation);
	}
}

class TokenVisitor : boost::static_visitor<> {

	public:
//		TokenVisitor(std::vector<std::string> &n_dst) : m_dst(n_dst) {}

		void operator()(const std::string &n_string_token) {

			case_permutations(n_string_token, m_dst);
		}

		void operator()(const whitespace &) {

			m_dst.push_back("");
			m_dst.push_back(" ");
			m_dst.push_back("_");
			m_dst.push_back(".");
			m_dst.push_back("-");
			m_dst.push_back("\t");
		}

		void operator()(const string_var &n_str_var) {

			for (const std::string &option : n_str_var.m_options) {
				case_permutations(option, m_dst);
			}
		}

	public:
		std::vector<std::string> m_dst;
};


std::vector<std::string> recurse_permutations(const std::list<token> &n_remains) {

	std::list<token> remains = n_remains;

	// Get a vector with all the permutations for the first remaining token

	TokenVisitor v;
	boost::apply_visitor(v, remains.front());

	// Add all of them to the destination vector plus the

	remains.pop_front();

	// If we reached the last of tokens, we return all permutations of it
	if (remains.empty()) {

		return v.m_dst;

	// Otherwise we recurse further
	} else {

		std::vector<std::string> ret;

		// All permutations of all combinations after that. What a waste of memory...
		std::vector<std::string> recursed_strings = recurse_permutations(remains);

		for (const std::string &begin : v.m_dst) {
			for (const std::string &end : recursed_strings) {
				ret.push_back(begin + end);
			}
		}

		return ret;
	}
}


std::size_t generate_permutations(const std::string &n_input, std::vector<std::string> &n_output) {

	std::list<token> tokens = parse_input_string(n_input);

	// Now that I have a vector of tokens, iterate and generate output for each

	n_output = recurse_permutations(tokens);

	return n_output.size();
}




} // namespace rescue
} // namespace moose
