include(FetchContent)
FetchContent_Declare(
    iir
    URL https://github.com/berndporr/iir1/archive/refs/heads/master.zip
)
FetchContent_MakeAvailable(iir)

find_package(Threads REQUIRED)
find_package(ALSA REQUIRED)

add_executable(vocoder main.cc callbacks.cc)
target_link_libraries(vocoder PRIVATE iir::iir)
target_link_libraries(vocoder PRIVATE ALSA::ALSA)
target_link_libraries(vocoder PRIVATE Threads::Threads)
