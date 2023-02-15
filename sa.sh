CPP_FILES=$(find src -name "*.cc")
clang-tidy -p build $CPP_FILES