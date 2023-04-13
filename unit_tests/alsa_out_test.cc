#include "../include/alsa_out.hh"
#include <chrono>
#include <cmath>
#include <gtest/gtest.h>
#include <vector>

const int frames = 32;
const double sampling_rate = 44100;
const float pi = 3.14;
// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(AlsaOutTest, ThreadAndMessaging) {

    Pipe<Audio> in_pipe;

    AlsaOut a_out("plughw:2,0,0", in_pipe);

    a_out.run();

    std::thread input_thread{[&in_pipe] {
        int freq = 440;
        Audio sin_audio;

        auto start = std::chrono::steady_clock::now();

        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(5)) {

            for (int i = 0; i < frames; i++) {
                // stereo
                sin_audio.push_back(10000 * (std::sin(2. * i * pi * freq / sampling_rate)));
                sin_audio.push_back(10000 * (std::sin(2. * i * pi * freq / sampling_rate)));
            }

            auto *in = &in_pipe;
            {
                std::lock_guard<std::mutex> lk(in->cond_m);
                in->queue.push(sin_audio);
            }
            in->cond.notify_all();
        }
    }};

    std::cerr << "Output stopped" << std::endl;
    input_thread.join();
    std::cerr << "Input stopped" << std::endl;
}