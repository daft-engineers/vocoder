add_library(vocoder_functionality callbacks.cc filter.cc amp.cc)
target_link_libraries(vocoder_functionality iir::iir_static)
target_link_libraries(vocoder_functionality ALSA::ALSA)
target_link_libraries(vocoder_functionality Threads::Threads)

add_executable(vocoder main.cc)
target_link_libraries(vocoder vocoder_functionality)
