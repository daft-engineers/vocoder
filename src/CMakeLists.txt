include(FetchContent)
FetchContent_Declare(
    iir
    URL https://github.com/berndporr/iir1/archive/refs/heads/master.zip
)
FetchContent_MakeAvailable(iir)

add_executable(vocoder main.cc)
target_link_libraries(vocoder iir::iir)
