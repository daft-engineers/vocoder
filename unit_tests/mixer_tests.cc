#include "../include/mixer/mixer.hh"
#include <array>
#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>
#include <thread>

// This is a unit test, magic numbers are called test data here
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(MixerTests, Summation) {
    std::array<Audio, 2> inputs{{{1, 2, 3, 4, 5, 6}, {6, 5, 4, 3, 2, 1}}};
    Audio expected{{7, 7, 7, 7, 7, 7}};
    Audio result = mixer::Mixer<2>::sum(inputs);
    ASSERT_EQ(result, expected);
}

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(MixerTests, Integration) {
    // set up mixer with 2 inputs
    Pipe<Audio> output{};
    std::array<Pipe<Audio>, 2> inputs{};
    mixer::Mixer<2> mixer(inputs, output);

    // thread sync stuff
    std::condition_variable input_sync;
    std::mutex input_sync_mutex;
    bool input_sync_ready = false;

    // start mixer
    mixer.run();

    // provide input
    std::thread input_thread1{[&inputs] {
        auto *pipe = &inputs[0];
        Audio sample{{10, 1, 34, 29, 98}};

        {
            std::lock_guard<std::mutex> lk(pipe->cond_m);
            pipe->queue.push(sample);
        }
        pipe->cond.notify_all();
    }};

    std::thread input_thread2{[&inputs, &input_sync, &input_sync_mutex, &input_sync_ready] {
        auto *pipe = &inputs[1];
        Audio sample{{23, 39, 83, 38, 28}};

        std::unique_lock<std::mutex> lk(input_sync_mutex);
        input_sync.wait(lk, [&input_sync_ready] { return input_sync_ready; });

        {
            std::lock_guard<std::mutex> lk(pipe->cond_m);
            pipe->queue.push(sample);
        }
        pipe->cond.notify_all();
    }};

    // listen for output
    std::thread output_thread{[&output] {
        Audio expected{{33, 40, 117, 67, 126}};
        std::unique_lock<std::mutex> lk(output.cond_m);
        output.cond.wait(lk, [&output] { return output.queue.empty() == false; });
        ASSERT_EQ(output.queue.front(), expected);
    }};

    // stop mixer thread, then release lock on input data
    mixer.stop();
    {
        std::lock_guard<std::mutex> lk(input_sync_mutex);
        input_sync_ready = true;
    }
    input_sync.notify_all();

    input_thread1.join();
    input_thread2.join();
    output_thread.join();
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
