// Copyright 2019 Stephan Menzel. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "BruteForceLuks.hpp"

#include "tools/Log.hpp"
#include "tools/Assert.hpp"

#include <libcryptsetup.h>

namespace moose {
namespace rescue {

using namespace moose::tools;
namespace fs = boost::filesystem;

void crypt_log_callback(int n_level, const char *n_msg, void *) {

	switch (n_level) {
		case CRYPT_LOG_ERROR:
			BOOST_LOG_SEV(logger(), error) << "Crypt: " << n_msg;
			break;
		default:
		case CRYPT_LOG_NORMAL:
			BOOST_LOG_SEV(logger(), normal) << "Crypt: " << n_msg;
			break;
		case CRYPT_LOG_VERBOSE:
		case CRYPT_LOG_DEBUG_JSON:
		case CRYPT_LOG_DEBUG:
			BOOST_LOG_SEV(logger(), debug) << "Crypt: " << n_msg;
			break;
	}
}

bool attempt_password(const boost::filesystem::path &n_header_file, const std::string &n_password) {

	std::string header_file_name{ n_header_file.string() };

	int ret = 0;
	struct crypt_device *crypt_dev = nullptr;


	// Load the LUKS volume header
	crypt_init(&crypt_dev, header_file_name.c_str());
	crypt_load(crypt_dev, CRYPT_LUKS1, nullptr);

	crypt_set_log_callback(crypt_dev, &crypt_log_callback, &ret);

	// Decrypt the LUKS volume with the password
	ret = crypt_activate_by_passphrase(crypt_dev, nullptr, CRYPT_ANY_SLOT, n_password.c_str(), n_password.size(), CRYPT_ACTIVATE_READONLY);
	if (ret >= 0) {
		// We have a positive result. Who would have guessed?
		BOOST_LOG_SEV(logger(), normal) << "Activation returned " << ret << ". Seems like we have a winner: " << n_password;
		crypt_free(crypt_dev);
		return true;
	} else {
		BOOST_LOG_SEV(logger(), debug) << "Failed to open volume";
		crypt_free(crypt_dev);
		return false;
	}
}

} // namespace rescue
} // namespace moose
