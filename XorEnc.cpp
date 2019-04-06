// Copyright 2019 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "XorEnc.hpp"

namespace moose {
namespace rescue {

/* courtesy of
   https://kylewbanks.com/blog/Simple-XOR-Encryption-Decryption-in-Cpp
   All credits to Kyle Banks
 */
std::string encrypt_decrypt(const std::string &n_source) {

	char key[3] = {'W', 'T', 'F'};
	std::string output{ n_source };

	for (unsigned int i = 0; i < n_source.size(); i++) {
		output[i] = n_source[i] ^ key[i % (sizeof(key) / sizeof(char))];
	}

	return output;
}

}
}
