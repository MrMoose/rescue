
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
	WorkQueue.cpp
	XorEnc.cpp
	Sha512.cpp
	BruteForceLuks.cpp
)

set(RESCUE_HDR
	RescueConfig.hpp
	Types.hpp
	InputParser.hpp
	InputGenerator.hpp
	WorkQueue.hpp
	XorEnc.hpp
	Sha512.hpp
	BruteForceLuks.hpp
)

# Find cryptsetup in order to include it.
# This of course makes us platform dependent
find_package(PkgConfig REQUIRED)
pkg_check_modules(CRYPTSETUP REQUIRED libcryptsetup)


add_library(rescue ${RESCUE_SRC} ${RESCUE_HDR})
if (${BUILD_SHARED_LIBS})
	target_compile_definitions(rescue PUBLIC -DRESCUE_DLL)
endif()

target_link_libraries(rescue
	PUBLIC
		moose_tools
		Boost::boost
	PRIVATE
		mredis
		Boost::thread
		Boost::system
		${CRYPTSETUP_LIBRARIES}
)

target_include_directories(rescue
   PUBLIC
        $<INSTALL_INTERFACE:rescue>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        ${CRYPTSETUP_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set(RESCUE_SERVER_SRC
	rescue_server.cpp
	RescueServer.cpp
	)

set(RESCUE_SERVER_HDR
	RescueServer.hpp
	)

add_executable(rescue_server ${RESCUE_SERVER_SRC} ${RESCUE_SERVER_HDR})

target_link_libraries(rescue_server
	rescue
	Boost::program_options
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
	rescue
	mredis
	Boost::program_options
)

add_subdirectory(test)
enable_testing()
add_test(NAME Input     COMMAND TestInput     )
add_test(NAME Parser    COMMAND TestParser    )
add_test(NAME Enc       COMMAND TestEnc       )
add_test(NAME Sha512    COMMAND TestSha512    )


