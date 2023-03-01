#include "../../src/filter/filter.cc"
#include <cmath>
#include <gtest/gtest.h>
#include <vector>

// Test Filter constructor
TEST(FilterTest, Constructor) {
BPFilter f(2, 0, UINT_MAX);
// not sure what to test here yet
EXPECT_EQ(0, 0);
}

const unsigned int sample_size = 100;
const float threshold = 0.3;

// Test filter function at a range of frequencies
TEST(FilterTest, FrequencyResponse) {
const unsigned int centre = 75;
const unsigned int width = 20;
BPFilter f(2, centre, width);

// Low freq sin
const unsigned int low_freq = 10;
std::vector<float> low_sin;
// Pass freq sin
const unsigned int pass_freq = 75;
std::vector<float> pass_sin;
// Low freq sin
const unsigned int high_freq = 200;
std::vector<float> high_sin;

for (int i = 0; i < sample_size; i++) {
low_sin.push_back(sin(2 * i * M_PI * low_freq));
pass_sin.push_back(sin(2 * i * M_PI * pass_freq));
high_sin.push_back(sin(2 * i * M_PI * high_freq));
}

Audio low_sin_audio;
Audio pass_sin_audio;
Audio high_sin_audio;

low_sin_audio.sample = low_sin;
pass_sin_audio.sample = pass_sin;
high_sin_audio.sample = high_sin;

// this won't work because it's one filter (break out individual tests)
Audio low_out = f.filter(low_sin_audio);
Audio pass_out = f.filter(pass_sin_audio);
Audio high_out = f.filter(high_sin_audio);

auto max = 0;
for (int i = 0; i < sample_size; i++) {
if (low_out.sample[i] > threshold) {
printf("Low frequency not attentuated");
FAIL();
}
if (high_out.sample[i] > threshold) {
printf("High frequency not attentuated");
FAIL();
}
if (pass_out.sample[i] > max) {
max = pass_out.sample[i];
}
}

if (max < threshold) {
printf("Pass frequency attenuated");
FAIL();
}
}
