/* daft engineers vocoder project
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "../include/rms.hh"

RMS::RMS(int num_samples, Pipe<Audio> &input_pipe, Pipe<double> &output_pipe, std::chrono::milliseconds timeout)
    : input_pipe(input_pipe), output_pipe(output_pipe), timeout(timeout) {
    squared_sample_buffer.resize(num_samples);
}

void RMS::insert(Audio packet) {
    std::for_each(packet.begin(), packet.end(), [this](const int16_t item) {
        // this assumes that the 0 point for the signal is 0
        uint32_t squared = item * item;
        squared_sample_buffer[sample_buffer_index] = squared;
        sample_buffer_index = (sample_buffer_index + 1) % squared_sample_buffer.size();
    });
}

double RMS::calc() {
    uint64_t total = 0;
    for (auto &n : squared_sample_buffer) {
        total += n;
    }
    double output = std::sqrt(total / squared_sample_buffer.size()) / INT16_MAX;
    return output;
}

void RMS::run() {
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
    Scheduler_helper::set_thread_priority(thread.native_handle(), "rms");
}
