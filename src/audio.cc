#include "../include/audio/audio.hh"

Audio centre_zero(std::vector<uint16_t> &samples) {
    Audio out;
    for (int32_t sample : samples) {
        sample -= UINT16_MAX / 2;
        auto sign_sample = static_cast<int16_t>(sample);
        out.push_back(sign_sample);
    }
    return out;
}