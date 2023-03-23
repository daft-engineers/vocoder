#ifndef MIXER_MIXER_HH
#define MIXER_MIXER_HH

#include "../audio/audio.hh"
#include "../pipe.hh"
#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>

namespace mixer {

//  Mixer class takes in a list of input pipes, extracts the audio packet from
//  each of them. It then produces a new audio packet which is the sum of the
//  previous audio packets.
template <std::size_t num_banks> class Mixer {
  private:
    std::array<Pipe<Audio>, num_banks> &input_pipes;
    Pipe<Audio> &output_pipe;

    bool run_thread{};
    std::thread thread;

  public:
    Mixer(std::array<Pipe<Audio>, num_banks> &input_pipes, Pipe<Audio> &output_pipe)
        : input_pipes(input_pipes), output_pipe(output_pipe) {
    }

    void stop() {
        run_thread = false;
        thread.join();
    }

    static Audio sum(std::array<Audio, num_banks> audio_packets) {
        Audio output_packet;

        int num_samples = audio_packets[0].size();
        output_packet.resize(num_samples);

        for (int sample = 0; sample < num_samples; sample++) {
            for (auto packet : audio_packets) {
                output_packet[sample] += packet[sample];
            }
        }
        return output_packet;
    }

    void run() {
        run_thread = true;
        thread = std::thread([this]() {
            while (run_thread) {
                std::array<Audio, num_banks> audio_packets;
                for (int bank = 0; bank < num_banks; bank++) {
                    auto &pipe = input_pipes[bank];

                    std::unique_lock<std::mutex> lk(pipe.cond_m);
                    pipe.cond.wait(lk, [&pipe] { return pipe.queue.empty() == false; });

                    audio_packets[bank] = pipe.queue.front();
                    pipe.queue.pop();
                }

                Audio output_packet = sum(audio_packets);
                {
                    std::lock_guard<std::mutex> lk(output_pipe.cond_m);
                    output_pipe.queue.push(output_packet);
                }
                output_pipe.cond.notify_all();
            }
        });
    }
};

} // namespace mixer

#endif // MIXER_MIXER_HH
