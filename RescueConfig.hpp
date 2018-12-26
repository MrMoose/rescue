//  Copyright 2018 Stephan Menzel. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

// see https://gcc.gnu.org/wiki/Visibility

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
    #define RESCUE_DLL_IMPORT __declspec(dllimport)
    #define RESCUE_DLL_EXPORT __declspec(dllexport)
    #define RESCUE_DLL_LOCAL
#else
	#if __GNUC__ >= 4
        #define RESCUE_DLL_IMPORT __attribute__ ((visibility ("default")))
        #define RESCUE_DLL_EXPORT __attribute__ ((visibility ("default")))
        #define RESCUE_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
	#else
        #define RESCUE_DLL_IMPORT
        #define RESCUE_DLL_EXPORT
        #define RESCUE_DLL_LOCAL
	#endif
#endif

#ifdef RESCUE_DLL // defined if is compiled as a DLL
    #ifdef rescue_EXPORTS // defined if we are building the DLL (instead of using it)
        #define RESCUE_API RESCUE_DLL_EXPORT
	#else
        #define RESCUE_API RESCUE_DLL_IMPORT
    #endif // rescue_EXPORTS
    #define RESCUE_LOCAL RESCUE_DLL_LOCAL
#else // RESCUE_DLL is not defined: this means it is a static lib.
    #define RESCUE_API
    #define RESCUE_LOCAL
#endif // RESCUE_DLL



#ifdef _MSC_VER
#pragma warning (disable : 4996) // Function call with parameters that may be unsafe.
#endif
