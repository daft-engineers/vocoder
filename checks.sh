#! /bin/bash

set -e

INC=$(find include -name "*.hh")
SRC=$(find src \( -name "*.cc" -o -name "*.hh" \))
UNIT=$(find unit_tests -name "*.cc")
WRONG_EXT=$(find src unit_tests \( -name "*.cpp" -o -name "*.hpp" \))

if [[ $1 == "--static-analysis" ]]; then
    clang-tidy -p build --warnings-as-errors='clang-diagnostic-*,clang-analyzer-*,clang-analyzer-*,concurrency-*,cppcoreguidelines-*,llvm-*,modernize-*,-modernize-use-trailing-return-type,performance-*' $INC $SRC 
elif [[ $1 == "--formatting" ]]; then
    clang-format -n -Werror -style=file $INC $SRC $UNIT
    if [[ -n $WRONG_EXT ]]; then 
        echo "Files found ending in .cpp: $WRONG_EXT"
        exit 1
    fi
else
    clang-tidy -p build $INC $SRC 
    clang-format -i -style=file $INC $SRC $UNIT
    if [[ -n $WRONG_EXT ]]; then
        echo "Files found ending in .cpp: $WRONG_EXT"
    fi
fi
