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


add_subdirectory(src)
add_subdirectory(unit_tests)
target_include_directories(vocoder PUBLIC include)
