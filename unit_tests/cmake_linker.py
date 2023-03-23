import os
from os.path import isfile, join

def main():
    ignore = ['CMakeLists.txt', 'cmake_linker.py']

    preamble = """include(FetchContent)
    FetchContent_Declare(
        googletest
            URL https://github.com/google/googletest/archive/refs/heads/main.zip
            )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    enable_testing()
    
    include(GoogleTest)\n"""
    
    cwd = os.path.abspath(os.getcwd())
    testfiles = [f for f in os.listdir(cwd) if isfile(join(cwd, f)) and not f in ignore]

    with open('CMakeLists.txt', 'w') as cmake:
        cmake.write(preamble)

        for test in testfiles:
            cmake.write(f"\nadd_executable({test[:-3]} {test})\n")
            cmake.write(f"target_link_libraries({test[:-3]} GTest::gtest_main)\n")
            cmake.write(f"target_link_libraries({test[:-3]} vocoder_functionality)\n")
            cmake.write(f"gtest_discover_tests({test[:-3]})\n\n")



if __name__ == "__main__":
    main()
