
# Copyright 2015 Stephan Menzel. Distributed under the Boost
# Software License, Version 1.0. (See accompanying file
# LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_executable(TestInput TestInput.cpp)
target_link_libraries(TestInput librescue Boost::unit_test_framework)

add_executable(TestParser TestParser.cpp)
target_link_libraries(TestParser librescue Boost::unit_test_framework)

