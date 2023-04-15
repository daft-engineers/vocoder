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

namespace rms {

template <std::size_t num_samples> class RMS {
  private:
    Pipe<Audio> &input_pipe;
    Pipe<double> &output_pipe;

    std::array<uint32_t, num_samples> squared_sample_buffer = {0};
    int sample_buffer_index = 0;

    const std::chrono::milliseconds timeout;
    bool thread_alive{false};
    std::thread thread;

  public:
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
        uint64_t total = std::accumulate(std::begin(squared_sample_buffer), std::end(squared_sample_buffer), 0);
        double output = std::sqrt(total / num_samples);
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
