#ifndef MIXER_MIXER_HH
#define MIXER_MIXER_HH

#include "../audio/audio.hh"
#include "../pipe.hh"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <iostream>
#include <pthread.h>
#include <cstring>

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
    const std::chrono::milliseconds timeout;

  public:
    Mixer(std::array<Pipe<Audio>, num_banks> &input_pipes_, Pipe<Audio> &output_pipe_, std::chrono::milliseconds timeout_)
        : input_pipes(input_pipes_), output_pipe(output_pipe_), timeout(timeout_) {
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
        thread = std::thread([this]() {
            while (true) {
                //std::cerr << "Starting while loop" << std::endl;
                std::array<Audio, num_banks> audio_packets;
                //std::cout << "mixer";
                for (int bank = 0; bank < num_banks; bank++) {
                    //std::cerr << "Starting for loop" << std::endl;
                    auto &pipe = input_pipes[bank];

                    {
                        std::unique_lock<std::mutex> lk(pipe.cond_m);
                        if (!pipe.cond.wait_for(lk, timeout, [&pipe] { return pipe.queue.empty() == false; })) {
                            std::cerr << "Mixer dead" << std::endl;
                            std::quick_exit(-1);
                            return false;
                        }
                        //std::cerr << "Past wait" << std::endl;

                        audio_packets[bank] = pipe.queue.front();
                        //std::cout << " -\t" << pipe.queue.front()[0];
                        //std::cerr << "Hello" << audio_packets[0][0] << std::endl;
                        pipe.queue.pop();
                    }
                }
                //std::cout << std::endl;

                Audio output_packet = sum(audio_packets);
                {
                    //std::cerr << output_packet[0] << std::endl;
                    std::lock_guard<std::mutex> lk(output_pipe.cond_m);
                    output_pipe.queue.push(output_packet);
                }
                output_pipe.cond.notify_all();
            }
            return true;
        });
        sched_param sch;
        int policy;
        pthread_getschedparam(thread.native_handle(), &policy, &sch);
        sch.sched_priority = 20;
        if (pthread_setschedparam(thread.native_handle(), SCHED_FIFO, &sch)) {
            std::cerr << "Failed to set sched param: " << std::strerror(errno) << std::endl;
        } else {
            std::cerr << "Priority increased successfully (mixer)" << std::endl;
        }
    }

    void stop() {
        thread.join();
    }
};

} // namespace mixer

#endif // MIXER_MIXER_HH
