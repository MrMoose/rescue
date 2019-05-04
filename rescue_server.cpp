
//  Copyright 2019 Stephan Menzel. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "RescueServer.hpp"

#include "mredis/AsyncClient.hpp"

#include "tools/Log.hpp"
#include "tools/Error.hpp"

#include <boost/config.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#ifndef _WIN32
#include <sys/resource.h>
#endif

#include <chrono>
#include <iostream>
#include <cstdlib>
#include <string>

using namespace moose::mredis;
using namespace moose::tools;
using namespace moose::rescue;

namespace fs = boost::filesystem;


int main(int argc, char **argv) {
	
#ifndef _WIN32
	// core dumps may be disallowed by parent of this process; change that
	struct rlimit core_limits;
	core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &core_limits);
#endif

	moose::tools::init_logging();

	namespace po = boost::program_options;

	po::options_description desc("rescue server options");
	desc.add_options()
	    ("help,h",   "Print this help message")
	    ("server,s", po::value<std::string>()->default_value("127.0.0.1"), "give redis server ip")
	    ("file,f",   po::value<std::string>()->default_value("luks_header"), "input file name");

	try {
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << "Behold your options!\n";
			std::cout << desc << std::endl;
			return EXIT_SUCCESS;
		}

		const std::string server_ip_string = vm["server"].as<std::string>();
		const boost::filesystem::path filename = vm["file"].as<std::string>();

		// Check the file for accessibility
		boost::system::error_code errc;

		if (!fs::is_regular_file(filename, errc) || errc) {
			std::cerr << "Could not access LUKS header file '" << filename << "': " << errc;
			return EXIT_FAILURE;
		}

		RescueServer server(server_ip_string);
		server.run(filename);

		return EXIT_SUCCESS;

	} catch (const moose_error &merr) {
		std::cerr << "Exception executing rescue server: " << boost::diagnostic_information(merr) << std::endl;
	} catch (const boost::system::error_code &errc) {
		std::cerr << "Exception processing: " << boost::diagnostic_information(errc) << std::endl;
	} catch (const std::exception &sex) {
		std::cerr << "Unexpected exception reached main: " << sex.what() << std::endl;
	} catch (...) {
		std::cerr << "Unhandled error reached main function. Aborting" << std::endl;
	}

	return EXIT_FAILURE;
}
