#ifndef AUDIO_AUDIO_HH
#define AUDIO_AUDIO_HH

#include <cstdint>
#include <vector>

using Audio = std::vector<int16_t>;

Audio centre_zero(std::vector<uint16_t> &samples);

#endif // AUDIO_AUDIO_HH
