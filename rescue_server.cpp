
//  Copyright 2018 Stephan Menzel. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "InputGenerator.hpp"

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

	po::options_description desc("rescue client options");
	desc.add_options()
	    ("help,h",   "Print this help message")
	    ("server,s", po::value<std::string>()->default_value("127.0.0.1"), "give redis server ip")
	    ("file,f",   po::value<std::string>()->default_value("candidates.txt"), "input file name");

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

		// Connect to the redis backend to enter the candidate strings
		AsyncClient redis(server_ip_string);
		redis.connect();

		const fs::path infile(vm["file"].as<std::string>());

		if (!fs::exists(infile)) {
			std::cerr << "Input file " << infile << " does not exist" << std::endl;
			return EXIT_FAILURE;
		}

		// Open input file and read line by line, parse and enter into work queue
		fs::ifstream ifile(infile, std::ios::in);

		for (std::string line; std::getline(ifile, line); ) {

			std::cout << "input: " << line;

			std::vector<std::string> permutations;

			std::size_t num = generate_permutations(line, permutations);
			std::cout << " yielded " << num << " permutations" << std::endl;

			for (const std::string &candidate : permutations) {



			}
		}

		std::cout << "done" << std::endl;

		return EXIT_SUCCESS;

	} catch (const moose_error &merr) {
		std::cerr << "Exception executing test cases: " << boost::diagnostic_information(merr) << std::endl;
	} catch (const boost::system::error_code &errc) {
		std::cerr << "Exception processing: " << boost::diagnostic_information(errc) << std::endl;
	} catch (const std::exception &sex) {
		std::cerr << "Unexpected exception reached main: " << sex.what() << std::endl;
	} catch (...) {
		std::cerr << "Unhandled error reached main function. Aborting" << std::endl;
	}

	return EXIT_FAILURE;
}
