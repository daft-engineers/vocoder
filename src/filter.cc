/* daft engineers vocoder project
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "../include/filter.hh"
#include "../include/scheduler_helper.hh"

BPFilter::BPFilter(int order, double sampling_rate, double centre_freq_, double freq_range_, Pipe<Audio> &input_,
                   Pipe<Audio> &output_, std::chrono::milliseconds timeout_)
    : centre_freq(centre_freq_), freq_range(freq_range_), input(input_), output(output_), timeout(timeout_) {

    f.setup(order, sampling_rate, centre_freq, freq_range);
}

void BPFilter::stop() {
    if (thread_alive) {
        filter_thread.join();
    }
}

void BPFilter::run() {
    filter_thread = std::thread([this]() {
        std::unique_lock<std::mutex> lk(input.cond_m);
        lk.unlock();
        thread_alive = true;
        while (true) {
            lk.lock();
            if (!input.cond.wait_for(lk, timeout, [this] { return input.queue.empty() == false; })) {
                return false;
            }

            Audio audio_out = filter(input.queue.front());
            input.queue.pop();
            lk.unlock();

            {
                std::lock_guard<std::mutex> out_lk(output.cond_m);
                output.queue.push(audio_out);
            }
            output.cond.notify_all();
        }
        return true;
    });
    Scheduler_helper::set_thread_priority(filter_thread.native_handle(), "filter");
}

Audio BPFilter::filter(const Audio &in_audio) {

    Audio filtered_audio;
    // Recognisable human speech components exist above 1kHz
    const unsigned int voice_low_freq = 1000;
    const unsigned int voice_high_freq = 2000;

    for (auto sample : in_audio) {
        auto buf = static_cast<int16_t>(f.filter(sample));

        // Boost high frequency components to emphasise voice
        if (centre_freq > voice_low_freq) {
            buf *= 2;
        }
        if (centre_freq > voice_high_freq) {
            buf *= 4;
        }
        filtered_audio.push_back(buf);
    }

    return filtered_audio;
}
