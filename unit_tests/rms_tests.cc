/* daft engineers vocoder project
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "../include/rms.hh"
#include <cmath>
#include <gtest/gtest.h>
#include <mutex>
#include <thread>

// This is a unit test, magic numbers are called test data here
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(RMSTests, buffer) {
    Pipe<Audio> input{};
    Pipe<double> output{};
    RMS rms(4, 1, input, output, std::chrono::milliseconds(100));
    Audio audio{0};

    // check that rms is 0 at the beginning
    ASSERT_EQ(rms.calc(), 0);

    audio[0] = 16;
    rms.insert(audio);
    // buffer should now be 256, 0, 0, 0 giving rms of 8/IM
    ASSERT_EQ(rms.calc(), 8.0 / INT16_MAX);

    audio[0] = -16;
    rms.insert(audio);
    // buffer should now be 256, 256, 0, 0 giving rms of 8r2/IM
    ASSERT_EQ(rms.calc(), 8.0 * std::sqrt(2) / INT16_MAX);

    rms.insert(audio);
    rms.insert(audio);
    // buffer should now be 256, 256, 256, 256 giving rms of 16/IM
    ASSERT_EQ(rms.calc(), 16.0 / INT16_MAX);

    // insertions to the buffer from here should roll back to the front

    audio[0] = 0;

    for (int i = 0; i < 4; i++) {
        rms.insert(audio);
    }

    // buffer should now be 0, 0, 0, 0, giving rms of 0
    ASSERT_EQ(rms.calc(), 0);

    Audio sample{INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX};

    rms.insert(sample);

    // buffer should now be IM, IM, IM, IM, giving rms of 1
    ASSERT_EQ(rms.calc(), 1);
}

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(RMSTests, multiple) {
    Pipe<Audio> input{};
    Pipe<double> output{};
    std::vector<RMS> rmsarr;
    for (int i = 0; i < 4; i++) {

        rmsarr.emplace_back(4 * (i + 1), 1, input, output, std::chrono::milliseconds(100));
    }

    Audio audio{16};

    // check that rms is 0 at the beginning
    ASSERT_EQ(rmsarr[0].calc(), 0);

    audio[0] = 16;
    for (int i = 0; i < 4; i++) {
        rmsarr[i].insert(audio);
    }
    // buffer should now be 256, 0, 0, 0 giving rms of 8/IM
    ASSERT_EQ(rmsarr[0].calc(), 8.0 / INT16_MAX);

    // buffer should now be 256, 0, 0, 0, 0, 0, 0, 0 giving rms of 4r2/IM
    ASSERT_EQ(rmsarr[1].calc(), 4.0 * std::sqrt(2) / INT16_MAX);

    // buffer should now be 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 giving rms of 4/IM
    ASSERT_EQ(rmsarr[3].calc(), 4.0 / INT16_MAX);
}

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(RMSTests, gains) {
    Pipe<Audio> input{};
    Pipe<double> output{};
    std::vector<RMS> rmsarr;
    for (int i = 0; i < 4; i++) {

        rmsarr.emplace_back(4, i / 2.0, input, output, std::chrono::milliseconds(100));
    }

    Audio audio{16};

    // check that rms is 0 at the beginning
    ASSERT_EQ(rmsarr[0].calc(), 0);

    audio[0] = 16;
    for (int i = 0; i < 4; i++) {
        rmsarr[i].insert(audio);
    }
    // buffer should now be 256, 0, 0, 0 with gain 0 giving rms of 0
    ASSERT_EQ(rmsarr[0].calc(), 0);

    // buffer should now be 256, 0, 0, 0 with gain 0.5 giving rms of 4/IM
    ASSERT_EQ(rmsarr[1].calc(), 4.0 / INT16_MAX);

    // buffer should now be 256, 0, 0, 0 with gain 1 giving rms of 8/IM
    ASSERT_EQ(rmsarr[2].calc(), 8.0 / INT16_MAX);

    // buffer should now be 256, 0, 0, 0 with gain 1.5 giving rms of 12/IM
    ASSERT_EQ(rmsarr[3].calc(), 12.0 / INT16_MAX);
}

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(RMSTests, Integration) {
    Pipe<Audio> input{};
    Pipe<double> output{};
    RMS rms(4, 1, input, output, std::chrono::milliseconds(100));

    rms.run();

    std::thread input_thread{[&input] {
        auto &pipe = input;
        Audio sample{16, 0, 0, 0};

        {
            std::lock_guard<std::mutex> lk(pipe.cond_m);
            pipe.queue.push(sample);
        }
        pipe.cond.notify_all();
    }};

    std::thread output_thread{[&output] {
        std::unique_lock<std::mutex> lk(output.cond_m);
        output.cond.wait(lk, [&output] { return output.queue.empty() == false; });
        ASSERT_EQ(output.queue.front(), 8.0 / INT16_MAX);
    }};

    rms.stop();

    input_thread.join();
    output_thread.join();
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
