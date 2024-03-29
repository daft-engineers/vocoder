/* daft engineers vocoder project
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "../include/filter.hh"
#include <chrono>
#include <cstdint>
#include <gtest/gtest.h>
#include <vector>

// This is a unit test, magic numbers are called test data here
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

const unsigned int sample_size = 100;
const double sampling_rate = 2000;
const float threshold = 30;
const float pi = 3.14;

// Test filter function is not returning all zeros
// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(FilterTest, EmptyReturn) {
    const unsigned int centre = 75;
    const unsigned int width = 20;
    Pipe<Audio> in_pipe;
    Pipe<Audio> out_pipe;

    BPFilter f(2, sampling_rate, centre, width, in_pipe, out_pipe, std::chrono::milliseconds(100));

    // High freq sin
    const unsigned int freq = 75;
    Audio sin_audio;

    for (int i = 0; i < sample_size; i++) {
        sin_audio.push_back(100 * (std::sin(2. * i * pi * freq) + 1));
    }

    Audio out = f.filter(sin_audio);

    bool flag = false;
    for (int i = 0; i < sample_size; i++) {
        if (out[i] > threshold) {
            flag = true;
        }
    }

    EXPECT_TRUE(flag) << "Filter returned all zeros";
}

// Test that filters can be initialised in an array
// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(FilterTest, Vectorable) {
    int num_filters = 8;
    std::vector<BPFilter> filter_array;
    filter_array.reserve(num_filters);

    const unsigned int centre = 75;
    const unsigned int width = 20;
    Pipe<Audio> in_pipe;
    Pipe<Audio> out_pipe;

    for (int i = 0; i < num_filters; i++) {
        filter_array.emplace_back(2, sampling_rate, centre, width, in_pipe, out_pipe, std::chrono::milliseconds(100));
    }

    ASSERT_TRUE(filter_array.size() == 8);
}

// Test filter function at a frequency above the pass band
// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(FilterTest, HighFrequencyResponse) {
    const unsigned int centre = 75;
    const unsigned int width = 20;
    Pipe<Audio> in;
    Pipe<Audio> out;

    BPFilter f(2, sampling_rate, centre, width, in, out, std::chrono::milliseconds(100));

    // High freq sin
    const unsigned int high_freq = 200;
    Audio high_sin_audio;

    for (int i = 0; i < sample_size; i++) {
        high_sin_audio.push_back(100 * (std::sin(2. * i * pi * high_freq) + 1));
    }

    Audio high_out = f.filter(high_sin_audio);

    for (int i = 0; i < sample_size; i++) {
        std::cerr << "High frequency at amplitude: " << high_out[i] << std::endl;
        EXPECT_TRUE(high_out[i] < threshold) << "High frequency not attentuated";
    }
}

// Test filter function at a frequency in the pass band
// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(FilterTest, PassFrequencyResponse) {
    const unsigned int centre = 75;
    const unsigned int width = 20;
    Pipe<Audio> in;
    Pipe<Audio> out;

    BPFilter f(2, sampling_rate, centre, width, in, out, std::chrono::milliseconds(100));

    // High freq sin
    const unsigned int pass_freq = 75;
    Audio pass_sin_audio;

    for (int i = 0; i < sample_size; i++) {
        pass_sin_audio.push_back(100 * (std::sin(2. * i * pi * pass_freq) + 1));
    }

    Audio pass_out = f.filter(pass_sin_audio);

    float max = 0;
    for (int i = 0; i < sample_size; i++) {
        if (pass_out[i] > max) {
            max = pass_out[i];
        }
    }

    EXPECT_TRUE(max > threshold) << "Pass frequency attentuated";
}

// Test filter function at a frequency below the pass band
// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(FilterTest, LowFrequencyResponse) {
    const unsigned int centre = 75;
    const unsigned int width = 20;
    Pipe<Audio> in;
    Pipe<Audio> out;

    BPFilter f(2, sampling_rate, centre, width, in, out, std::chrono::milliseconds(100));

    // High freq sin
    const unsigned int low_freq = 30;
    Audio low_sin_audio;

    for (int i = 0; i < sample_size; i++) {
        low_sin_audio.push_back(100 * (std::sin(2. * i * pi * low_freq) + 1));
    }

    Audio low_out = f.filter(low_sin_audio);

    for (int i = 0; i < sample_size; i++) {
        // std::cerr << "Low frequency at amplitude: " << low_out[i] << std::endl;
        EXPECT_TRUE(low_out[i] < threshold) << "Low frequency not attentuated";
    }
}

// Test filter returns values above and below 0
// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(FilterTest, PositiveNegative) {
    const unsigned int centre = 75;
    const unsigned int width = 20;
    Pipe<Audio> in;
    Pipe<Audio> out;

    BPFilter f(2, sampling_rate, centre, width, in, out, std::chrono::milliseconds(100));

    // High freq sin
    const unsigned int high_freq = 75;
    Audio high_sin_audio;

    for (int i = 0; i < sample_size; i++) {
        high_sin_audio.push_back(100 * (std::sin(2. * i * pi * high_freq) + 1));
    }

    Audio filter_out = f.filter(high_sin_audio);

    bool above = false;
    bool below = false;

    for (int i = 0; i < sample_size; i++) {
        if (filter_out[i] > 45) {
            above = true;
        } else if (filter_out[i] < -45) {
            below = true;
        }
    }

    EXPECT_TRUE(above) << "Filter returned value above 0";
    EXPECT_TRUE(below) << "Filter returned value below 0";
}

// Test filter function runs in less than 0.5ms
// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(FilterTest, RunTimeTest) {
    const unsigned int centre = 75;
    const unsigned int width = 20;
    const unsigned int max_delay = 500;
    Pipe<Audio> in;
    Pipe<Audio> out;

    BPFilter f(2, sampling_rate, centre, width, in, out, std::chrono::milliseconds(100));

    // High freq sin
    const unsigned int freq = 30;
    Audio sin_audio;

    for (int i = 0; i < sample_size; i++) {
        sin_audio.push_back(100 * (std::sin(2. * i * pi * freq) + 1));
    }
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    f.filter(sin_audio);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    EXPECT_TRUE(diff.count() < max_delay) << "Time difference = " << diff.count() << "[µs]" << std::endl;
}

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(FilterTest, ThreadAndMessaging) {
    const unsigned int centre = 75;
    const unsigned int width = 20;

    Pipe<Audio> in_pipe;
    Pipe<Audio> out_pipe;

    BPFilter f(2, sampling_rate, centre, width, in_pipe, out_pipe, std::chrono::milliseconds(100));

    f.run();

    std::thread input_thread{[&in_pipe] {
        // Pass freq sin
        const unsigned int pass_freq = 75;
        Audio pass_sin_audio;

        for (int i = 0; i < sample_size; i++) {
            pass_sin_audio.push_back(100 * (std::sin(2. * i * pi * pass_freq) + 1));
        }

        auto *in = &in_pipe;
        {
            std::lock_guard<std::mutex> lk(in->cond_m);
            in->queue.push(pass_sin_audio);
        }
        in->cond.notify_all();
    }};

    // listen for output
    std::thread output_thread{[&out_pipe] {
        std::unique_lock<std::mutex> lk(out_pipe.cond_m);
        out_pipe.cond.wait(lk, [&out_pipe] { return out_pipe.queue.empty() == false; });

        Audio pass_out = out_pipe.queue.front();
        int16_t max = 0;
        for (int i = 0; i < sample_size; i++) {
            if (pass_out[i] > max) {
                max = pass_out[i];
            }
        }
        EXPECT_TRUE(max > threshold) << "Pass frequency attentuated";
    }};

    std::cerr << "Filter stopped" << std::endl;
    input_thread.join();
    std::cerr << "Input stopped" << std::endl;
    output_thread.join();
    std::cerr << "Output stopped" << std::endl;
    f.stop();
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
