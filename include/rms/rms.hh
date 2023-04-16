/* daft engineers vocoder project
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef RMS_RMS_HH
#define RMS_RMS_HH

#include "../audio/audio.hh"
#include "../pipe.hh"
#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>

/** rms namespace
 *  Contains the RMS class.
 */
namespace rms {

/*  RMS (Root Mean Square) Class.
 *  This class determins the average power of a signal.
 */
template <std::size_t num_samples> class RMS {
  private:
    // Input pipe for the class
    Pipe<Audio> &input_pipe;
    // Output pipe for the class
    Pipe<double> &output_pipe;

    // Ring buffer which contains the recent history of the signal after it has been squared.
    std::array<uint32_t, num_samples> squared_sample_buffer = {0};
    // Points to the current head of the buffer
    int sample_buffer_index = 0;

    // Determines how the long the thread should wait before exiting if no data is received.
    const std::chrono::milliseconds timeout;
    // Boolean variable to track if the thread is running
    bool thread_alive{false};

    // Contains the thread carrying out the calculation
    std::thread thread;

  public:
    /** RMS Constructor
    *   Initialises the input and output pipes, and the timeout.
    *   @param input_pipe The Pipe
    */
    RMS(Pipe<Audio> &input_pipe, Pipe<double> &output_pipe, std::chrono::milliseconds timeout)
        : input_pipe(input_pipe), output_pipe(output_pipe), timeout(timeout) {
    }

    void insert(Audio packet) {
        std::for_each(packet.begin(), packet.end(), [this](const int16_t item) {
            // this assumes that the 0 point for the signal is 0
            uint32_t squared = item * item;
            squared_sample_buffer[sample_buffer_index] = squared;
            sample_buffer_index = (sample_buffer_index + 1) % num_samples;
        });
    }

    double calc() {
        uint64_t total = 0;
        for (auto &n : squared_sample_buffer) {
            total += n;
        }
        double output = std::sqrt(total / num_samples) / INT16_MAX;
        return output;
    }

    void run() {
        thread = std::thread([this]() {
            thread_alive = true;
            while (true) {
                {
                    std::unique_lock<std::mutex> lk(input_pipe.cond_m);
                    if (!input_pipe.cond.wait_for(lk, timeout, [this] { return input_pipe.queue.empty() == false; })) {
                        return false;
                    }

                    insert(input_pipe.queue.front());
                    input_pipe.queue.pop();
                }

                double output_packet = calc();
                {
                    std::lock_guard<std::mutex> lk(output_pipe.cond_m);
                    output_pipe.queue.push(output_packet);
                }
                output_pipe.cond.notify_all();
            }
            return true;
        });
    }
    ~RMS() {
        if (thread_alive) {
            thread.join();
        }
    }

    RMS(const RMS &) = delete;
    RMS &operator=(const RMS &) = delete;
    RMS(RMS &&) = delete;
    RMS &operator=(RMS &&) = delete;
};
} // namespace rms

#endif // RMS_RMS_HH
