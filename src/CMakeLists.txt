add_library(alsa_callbacks callbacks.cc)

add_executable(vocoder main.cc)
target_link_libraries(vocoder PRIVATE iir::iir_static)
target_link_libraries(vocoder PRIVATE ALSA::ALSA)
target_link_libraries(vocoder PRIVATE Threads::Threads)
target_link_libraries(vocoder PRIVATE alsa_callbacks)
