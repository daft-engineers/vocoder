ROOT_PATH=$(pwd)

if [ -d "$ROOT_PATH/build" ]; then
    echo "Build dir already exists... clearing"
    rm -r "$ROOT_PATH/build/"
    mkdir "$ROOT_PATH/build"
else
    echo "Creating build dir"
    mkdir "$ROOT_PATH/build"
fi

echo "Configuring cmake"
cd build
cmake "$ROOT_PATH" -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_CXX_FLAGS="-Werror -Wall"
cd "$ROOT_PATH"

echo "Done!"
echo "To build all and run tests, do the following:
$ cd build && make && cd unit_tests && ctest"
