#! /bin/bash

# daft engineers vocoder project
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

set -e

CURRENTVER="$(clang-format --version | grep " [0-9]*\." -o | grep "[0-9]*" -o)"
REQVER="15"

if [[ CURRENTVER -lt REQVER ]]; then
    echo "clang-format version below $REQVER detected. Version $REQVER or greater is requred."
    exit 1
fi

INC=$(find include -name "*.hh")
SRC=$(find src -name "*.cc")
UNIT=$(find unit_tests -name "*.cc")
WRONG_EXT=$(find src unit_tests \( -name "*.cpp" -o -name "*.hpp" \))
WRONG_LOCATION=$(find src -name "*.hh" && find include -name "*.cc")

if [[ $1 == "--static-analysis" ]]; then
    clang-tidy -p build $INC $SRC 
elif [[ $1 == "--formatting" ]]; then
    clang-format -n -Werror -style=file $INC $SRC $UNIT
    if [[ -n $WRONG_EXT ]]; then 
        echo "Files found ending in .cpp: $WRONG_EXT"
        exit 1
    elif [[ -n $WRONG_LOCATION ]]; then 
	echo "Header files found in src or cc files found in include: $WRONG_LOCATION"
	exit 1
    fi
else
    clang-tidy -p build $INC $SRC 
    clang-format -i -style=file $INC $SRC $UNIT
    if [[ -n $WRONG_EXT ]]; then
        echo "Files found ending in .cpp: $WRONG_EXT"
    elif [[ -n $WRONG_LOCATION ]]; then 
	echo "Header files found in src or cc files found in include: $WRONG_LOCATION"
	exit 1
    fi
fi
