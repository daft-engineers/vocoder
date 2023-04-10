#ifndef AUDIO_AUDIO_HH
#define AUDIO_AUDIO_HH

#include <cstdint>
#include <vector>

using Audio = std::vector<int16_t>;

Audio centre_zero(std::vector<uint16_t> samples) {
    Audio out;
    for (int32_t sample : samples) {
        sample -= UINT16_MAX / 2;
        sample = static_cast<int16_t>(sample);
        out.push_back(sample);
    }
}

#endif // AUDIO_AUDIO_HH
