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

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	)

add_executable(vocoder main.cpp)

enable_testing()

add_executable(
    example_test
    unit_tests/example_test.cc
    )
target_link_libraries(
    example_test
    GTest::gtest_main
    )	

include(GoogleTest)
	gtest_discover_tests(example_test)

