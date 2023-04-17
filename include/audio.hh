#ifndef AUDIO_HH
#define AUDIO_HH

#include <cstdint>
#include <vector>

/**
 * Vector wrapper to hold audio data as it transitions between modules.
*/
using Audio = std::vector<int16_t>;

#endif // AUDIO_HH
