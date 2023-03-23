#include "../src/filter.cc"
#include "../include/filter/filter.hh"
#include <cmath>
#include <gtest/gtest.h>
#include <vector>
#include <chrono>

const unsigned int sample_size = 100;
const double sampling_rate = 2000;
const float threshold = 30;
const float pi = 3.14;

// Test Filter constructor
/*TEST(FilterTest, Constructor) {
    BPFilter f(2, sampling_rate, 0, UINT_MAX);
    // not sure what to test here yet
    EXPECT_EQ(0, 0);
}

// Test filter function is not returning all zeros
TEST(FilterTest, EmptyReturn) {
    const unsigned int centre = 75;
    const unsigned int width = 20;

    BPFilter f(2, sampling_rate, centre, width);

    // High freq sin
    const unsigned int freq = 75;
    Audio sin_audio;

    for (int i = 0; i < sample_size; i++) {
        sin_audio.sample.push_back(sin(2 * i * pi * freq));
    }

    Audio out = f.filter(sin_audio);

    bool flag = false;
    for (int i = 0; i < sample_size; i++) {
        if (out.sample[i] > threshold) {
            flag = true;
        }
    }

    EXPECT_TRUE(flag) << "Filter returned all zeros";
}

// Test filter function at a frequency above the pass band
TEST(FilterTest, HighFrequencyResponse) {
    const unsigned int centre = 75;
    const unsigned int width = 20;

    BPFilter f(2, sampling_rate, centre, width);

    // High freq sin
    const unsigned int high_freq = 200;
    Audio high_sin_audio;

    for (int i = 0; i < sample_size; i++) {
        high_sin_audio.sample.push_back(sin(2 * i * pi * high_freq));
    }

    Audio high_out = f.filter(high_sin_audio);

    for (int i = 0; i < sample_size; i++) {
        EXPECT_TRUE(high_out.sample[i] < threshold) << "High frequency not attentuated";
    }
}

// Test filter function at a frequency in the pass band
TEST(FilterTest, PassFrequencyResponse) {
    const unsigned int centre = 75;
    const unsigned int width = 20;

    BPFilter f(2, sampling_rate, centre, width);

    // High freq sin
    const unsigned int pass_freq = 75;
    Audio pass_sin_audio;

    for (int i = 0; i < sample_size; i++) {
        pass_sin_audio.sample.push_back(sin(2 * i * pi * pass_freq));
    }

    Audio pass_out = f.filter(pass_sin_audio);

    float max = 0;
    for (int i = 0; i < sample_size; i++) {
        if (pass_out.sample[i] > max) {
            max = pass_out.sample[i];
        }
    }

    EXPECT_TRUE(max > threshold) << "Pass frequency attentuated";
}

// Test filter function at a frequency below the pass band
TEST(FilterTest, LowFrequencyResponse) {
    const unsigned int centre = 75;
    const unsigned int width = 20;

    BPFilter f(2, sampling_rate, centre, width);

    // High freq sin
    const unsigned int low_freq = 30;
    Audio low_sin_audio;

    for (int i = 0; i < sample_size; i++) {
        low_sin_audio.sample.push_back(sin(2 * i * pi * low_freq));
    }

    Audio low_out = f.filter(low_sin_audio);

    for (int i = 0; i < sample_size; i++) {
        EXPECT_TRUE(low_out.sample[i] < threshold) << "Low frequency not attentuated";
    }
}

// Test filter function runs in less than 0.5ms
TEST(FilterTest, RunTimeTest) {
    const unsigned int centre = 75;
    const unsigned int width = 20;
    const unsigned int max_delay = 500;

    BPFilter f(2, sampling_rate, centre, width);

    // High freq sin
    const unsigned int freq = 30;
    Audio sin_audio;

    for (int i = 0; i < sample_size; i++) {
        sin_audio.sample.push_back(sin(2 * i * pi * freq));
    }
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    f.filter(sin_audio);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    EXPECT_TRUE(diff.count() < max_delay) << "Time difference = " << diff.count() << "[µs]" << std::endl;
}*/

TEST(FilterTest, CallbackTest) {
    const unsigned int centre = 75;
    const unsigned int width = 20;

    Pipe<Audio> in_pipe;
    Pipe<Audio> out_pipe;

    BPFilter f(2, sampling_rate, centre, width, in_pipe, out_pipe);

    f.call_back();

    std::thread input_thread{[&in_pipe] {
        // Pass freq sin
        const unsigned int pass_freq = 75;
        Audio pass_sin_audio;

        for (int i = 0; i < sample_size; i++) {
            pass_sin_audio.push_back(100 * sin(2 * i * pi * pass_freq));
        }

        auto *in = &in_pipe;
        {
            std::lock_guard <std::mutex> lk(in->cond_m);
            in->queue.push(pass_sin_audio);
        }
        in->cond.notify_all();
    }};

    // listen for output
    std::thread output_thread{[&out_pipe] {
        std::unique_lock<std::mutex> lk(out_pipe.cond_m);
        out_pipe.cond.wait(lk, [&out_pipe] { return out_pipe.queue.empty() == false; });

        Audio pass_out = out_pipe.queue.front();
        unsigned short max = 0;
        for (int i = 0; i < sample_size; i++) {
            if (pass_out[i] > max) {
                max = pass_out[i];
            }
        }
        EXPECT_TRUE(max > threshold) << "Pass frequency attentuated";
    }};

    input_thread.join();
    output_thread.join();
    f.stop();
}
