
# Copyright 2015 Stephan Menzel. Distributed under the Boost
# Software License, Version 1.0. (See accompanying file
# LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_executable(TestInput TestInput.cpp)
target_link_libraries(TestInput rescue Boost::unit_test_framework)

add_executable(TestParser TestParser.cpp)
target_link_libraries(TestParser rescue Boost::unit_test_framework)

add_executable(TestEnc TestEnc.cpp)
target_link_libraries(TestEnc rescue Boost::unit_test_framework)

add_executable(TestSha512 TestSha512.cpp)
target_link_libraries(TestSha512 rescue Boost::unit_test_framework)

