#! /bin/bash

# daft engineers vocoder project
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

set -e

ROOT_PATH=$(pwd)

if [ -d "$ROOT_PATH/build" ]; then
    echo "Build dir already exists... clearing"
    rm -r "$ROOT_PATH/build/"
    mkdir "$ROOT_PATH/build"
else
    echo "Creating build dir"
    mkdir "$ROOT_PATH/build"
fi

echo "Linking unit tests"
cd unit_tests
python3 cmake_linker.py
cd "$ROOT_PATH"

echo "Configuring cmake"
cd build
cmake "$ROOT_PATH" -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_CXX_FLAGS="-Werror -Wall"
cd "$ROOT_PATH"

echo "Done!"
echo "To build all and run tests, do the following:
$ cd build && make && cd unit_tests && ctest"
