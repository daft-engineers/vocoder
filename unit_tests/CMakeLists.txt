include(FetchContent)
FetchContent_Declare(
	googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
        )

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include(GoogleTest)

add_executable(example_test example_test.cc)
target_link_libraries(example_test GTest::gtest_main)
target_link_libraries(example_test iir::iir)
gtest_discover_tests(example_test)

