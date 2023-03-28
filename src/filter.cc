#include "../include/filter/filter.hh"

BPFilter::BPFilter(int order, double sampling_rate, double centre_freq_, double freq_range_, Pipe<Audio> &input_,
                   Pipe<Audio> &output_)
    : centre_freq(centre_freq_), freq_range(freq_range_), input(input_), output(output_) {

    f.setup(order, sampling_rate, centre_freq, freq_range);
}
BPFilter::~BPFilter() {
    running = false;
    if (thread_alive) {
        filter_thread.join();
    }
}

void BPFilter::run() {
    filter_thread = std::thread([this]() {
        std::unique_lock<std::mutex> lk(input.cond_m);
        running = true;
        thread_alive = true;
        while (running) {
            input.cond.wait(lk, [this] { return input.queue.empty() == false; });

            Audio audio_out = filter(input.queue.front());
            input.queue.pop();

            {
                std::lock_guard<std::mutex> out_lk(output.cond_m);
                output.queue.push(audio_out);
            }
            output.cond.notify_all();
        }
    });
}

void BPFilter::stop() {
    running = false;
}

Audio BPFilter::filter(const Audio &in_audio) {

    Audio filtered_audio;
    for (auto sample : in_audio) {
        auto buf = static_cast<signed short>(f.filter(sample));
        if (buf <= 0) {
            filtered_audio.push_back(0);
        } else {
            filtered_audio.push_back(buf);
        }
    }

    return filtered_audio;
}