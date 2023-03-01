#include "../../src/filter/filter.cc"
#include <cmath>
#include <gtest/gtest.h>
#include <vector>

const unsigned int sample_size = 100;
const double sampling_rate = 2000;
const float threshold = 0.3;
const float pi = 3.14;

// Test Filter constructor
TEST(FilterTest, Constructor) {
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
