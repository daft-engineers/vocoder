# daft engineers vocoder project
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
add_library(vocoder_functionality callbacks.cc filter.cc amp.cc rms.cc alsa_out.cc scheduler_helper.cc)
target_link_libraries(vocoder_functionality iir::iir_static)
target_link_libraries(vocoder_functionality ALSA::ALSA)
target_link_libraries(vocoder_functionality Threads::Threads)

add_executable(vocoder main.cc)
target_link_libraries(vocoder vocoder_functionality)
