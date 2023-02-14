# daft engineers vocoder project
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.16)

# project name and version
project(vocoder VERSION 0.0)

# set cpp standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	  RESULT_VARIABLE result
	    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
	      message(FATAL_ERROR "CMake step for googletest failed: ${result}")
      endif()
      execute_process(COMMAND ${CMAKE_COMMAND} --build .
	        RESULT_VARIABLE result
		  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
	  if(result)
		    message(FATAL_ERROR "Build step for googletest failed: ${result}")
	    endif()

	    # Prevent overriding the parent project's compiler/linker
	    # settings on Windows
	    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

	    # Add googletest directly to our build. This defines
	    # the gtest and gtest_main targets.
	    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
		                     ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
				                      EXCLUDE_FROM_ALL)

add_executable(vocoder src/main.cpp)
target_link_libraries(vocoder gtest_main)
add_test(NAME unit_tests/filter_test COMMAND vocoder)
