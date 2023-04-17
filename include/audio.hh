/* daft engineers vocoder project
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */#ifndef AUDIO_HH
#define AUDIO_HH

#include <cstdint>
#include <vector>

/**
 * Vector wrapper to hold audio data as it transitions between modules.
 */
using Audio = std::vector<int16_t>;

#endif // AUDIO_HH
