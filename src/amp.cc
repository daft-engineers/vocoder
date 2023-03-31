#include "../include/amp/amp.hh"
#include <cmath>

Audio Amplifier::amplify(Audio &sample, double amount) {
    Audio processed;
    for (auto point : sample) {

        processed.push_back((int16_t)(point * amount));
    }
    return processed;
}

Amplifier::Amplifier(Pipe<Audio> &input_, Pipe<double> &scaling_, Pipe<Audio> &output_)
    : input(input_), scaling(scaling_), output(output_) {
}

Amplifier::~Amplifier() {
    if (thread_alive) {
        amplifier_thread.join();
    }
}

void Amplifier::run() {
    amplifier_thread = std::thread([this]() {
        std::unique_lock<std::mutex> input_lk(input.cond_m);
        std::unique_lock<std::mutex> scaling_lk(scaling.cond_m);
        input_lk.unlock();
        scaling_lk.unlock();
        thread_alive = true;
        while (true) {
            input_lk.lock();
            scaling_lk.lock();
            if (!input.cond.wait_for(input_lk, timeout, [this] { return input.queue.empty() == false; })) {
                return false;
            }
            if (!scaling.cond.wait_for(scaling_lk, timeout, [this] { return scaling.queue.empty() == false; })) {
                return false;
            }

            Audio audio_out = amplify(input.queue.front(), scaling.queue.front());
            input.queue.pop();
            input_lk.unlock();
            scaling_lk.unlock();

            {
                std::lock_guard<std::mutex> out_lk(output.cond_m);
                output.queue.push(audio_out);
            }
            output.cond.notify_all();
        }
        return true;
    });
}
