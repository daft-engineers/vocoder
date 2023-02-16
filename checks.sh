set -e

SRC=$(find src -name "*.cc")
UNIT=$(find unit_tests -name "*.cc")

if [[ $1 == "--static-analysis" ]]; then
    clang-tidy -p build --warnings-as-errors='clang-diagnostic-*,clang-analyzer-*,clang-analyzer-*,concurrency-*,cppcoreguidelines-*,llvm-*,modernize-*,-modernize-use-trailing-return-type,performance-*' $SRC
elif [[ $1 == "--formatting" ]]; then
    clang-format -n -Werror -style=file $SRC $UNIT
else
    clang-tidy -p build $SRC
    clang-format -i -style=file $SRC $UNIT
fi