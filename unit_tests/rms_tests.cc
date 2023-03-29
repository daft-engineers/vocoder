/* daft engineers vocoder project
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "../include/rms/rms.hh"
#include <gtest/gtest.h>
#include <mutex>
#include <thread>

// This is a unit test, magic numbers are called test data here
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
TEST(RMSTests, buffer) {
    Pipe<Audio> input{};
    Pipe<double> output{};
    rms::RMS<4> rms(input, output);
    Audio audio{0};

    // check that rms is 0 at the beginning
    ASSERT_EQ(rms.calc(), 0);

    audio[0] = UINT16_MAX / 2 + 16;
    rms.insert(audio);
    // buffer should now be 16, 0, 0, 0 giving rms of 8
    ASSERT_EQ(rms.calc(), 8);

    audio[0] = UINT16_MAX / 2 - 16;
    rms.insert(audio);
    // buffer should now be 16, 16, 0, 0 giving rms of 8r2
    ASSERT_EQ(rms.calc(), 8 * std::sqrt(2));

    rms.insert(audio);
    rms.insert(audio);
    // buffer should now be 16, 16, 16, 16 giving rms of 16
    ASSERT_EQ(rms.calc(), 16);

    // insertions to the buffer from here should roll back to the front

    audio[0] = UINT16_MAX / 2;

    for (int i = 0; i < 4; i++) {
        rms.insert(audio);
    }

    // buffer should now be 0, 0, 0, 0, giving rms of 0
    ASSERT_EQ(rms.calc(), 0);
}

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(RMSTests, Integration) {
    Pipe<Audio> input{};
    Pipe<double> output{};
    rms::RMS<4> rms(input, output);

    rms.run();

    std::thread input_thread{[&input] {
        auto *pipe = &input;
        Audio sample{UINT16_MAX / 2 + 16, UINT16_MAX / 2 + 0, UINT16_MAX / 2 + 0, UINT16_MAX / 2 + 0};

        {
            std::lock_guard<std::mutex> lk(pipe->cond_m);
            pipe->queue.push(sample);
        }
        pipe->cond.notify_all();
    }};

    std::thread output_thread{[&output] {
        std::unique_lock<std::mutex> lk(output.cond_m);
        output.cond.wait(lk, [&output] { return output.queue.empty() == false; });
        ASSERT_EQ(output.queue.front(), 8.0f);
    }};

    rms.stop();
    input_thread.join();
    output_thread.join();
    std::cerr << "all threads joined\n";
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)