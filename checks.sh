set -e

CPP_FILES=$(find src -name "*.cc")
clang-tidy -p build $CPP_FILES

CPP_FILES+=" "
CPP_FILES+=$(find unit_tests -name "*.cc")
clang-format -i -style=file $CPP_FILES