
# Copyright 2018 Stephan Menzel. Distributed under the Boost
# Software License, Version 1.0. (See accompanying file
# LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.6)
project(rescue)

find_package(Boost 1.67.0 REQUIRED COMPONENTS chrono thread system fiber program_options log unit_test_framework)

set(RESCUE_SRC
	Types.cpp
	InputParser.cpp
	InputGenerator.cpp
	)

set(RESCUE_HDR
	RescueConfig.hpp
	Types.hpp
	InputParser.hpp
	InputGenerator.hpp
	)

add_library(librescue ${RESCUE_SRC} ${RESCUE_HDR})
if (${BUILD_SHARED_LIBS})
	target_compile_definitions(librescue PUBLIC -DRESCUE_DLL)
endif()

target_link_libraries(librescue
	PUBLIC
	    moose_tools
		Boost::boost
	PRIVATE
	    mredis
		Boost::thread
		Boost::system
)

target_include_directories(librescue
   PUBLIC
        $<INSTALL_INTERFACE:librescue>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set(RESCUE_SERVER_SRC
	RescueServer.cpp
	)

set(RESCUE_SERVER_HDR
	RescueServer.hpp
	)

add_library(rescue_server ${RESCUE_SERVER_SRC} ${RESCUE_SERVER_HDR})
if (${BUILD_SHARED_LIBS})
	target_compile_definitions(rescue_server PUBLIC -DRESCUE_SERVER_DLL)
endif()

target_link_libraries(rescue_server
	PUBLIC
	    librescue
		moose_tools
		Boost::boost
	PRIVATE
		mredis
		Boost::thread
		Boost::system
)

target_include_directories(rescue_server
   PUBLIC
        $<INSTALL_INTERFACE:rescue_server>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(rescue_client rescue_client.cpp)
target_link_libraries(rescue_client
	librescue
	mredis
	Boost::program_options
)

add_subdirectory(test)
enable_testing()
add_test(NAME Input     COMMAND TestInput     )

