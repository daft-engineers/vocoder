#ifndef MIXER_HH
#define MIXER_HH

#include "audio.hh"
#include "pipe.hh"
#include "scheduler_helper.hh"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>

/** mixer namespace
 *  Contains the mixer class.
 */
namespace mixer {
/** Mixer class
 *  Takes in a list of input pipes, extracts the audio packet from
 *  each of them. It then produces a new audio packet which is the sum of the
 *  previous audio packets.
 */
template <std::size_t num_banks> class Mixer {
  private:
    // Array of pipes for the input to the class
    std::array<Pipe<Audio>, num_banks> &input_pipes;
    // Single pipe as output for the class
    Pipe<Audio> &output_pipe;

    // The thread carrying out the calculations
    std::thread thread;
    // Determines how the long the thread should wait before exiting if no data is received.
    const std::chrono::milliseconds timeout;

  public:
    /**
     * Constructs a new mixer. Initialises the input and output pipes and timeout.
     * @param input_pipes_ Input pipe array.
     * @param output_pipe_ Output pipe.
     * @param timeout_ Time in milliseconds the thread will wait before exiting if no data provided.
     */
    Mixer(std::array<Pipe<Audio>, num_banks> &input_pipes_, Pipe<Audio> &output_pipe_,
          std::chrono::milliseconds timeout_)
        : input_pipes(input_pipes_), output_pipe(output_pipe_), timeout(timeout_) {
    }

    /**
     * Sums the samples at matching points on each sub array and combines it in to a single array of samples
     *
     * @param audio_packets Array of arrays of samples.
     * @return Summed array of samples.
     */
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
    /**
     * Creates the summing thread then returns.
     */
    void run() {
        thread = std::thread([this]() {
            while (true) {
                std::array<Audio, num_banks> audio_packets;
                for (int bank = 0; bank < num_banks; bank++) {
                    auto &pipe = input_pipes[bank];

                    std::unique_lock<std::mutex> lk(pipe.cond_m);
                    if (!pipe.cond.wait_for(lk, timeout, [&pipe] { return pipe.queue.empty() == false; })) {
                        return false;
                    }

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
            return true;
        });
        Scheduler_helper::set_thread_priority(thread.native_handle(), "mixer");
    }
    /**
     * Join the mixer thread when the destructor is called
     */
    void stop() {
        thread.join();
    }
};

} // namespace mixer

#endif // MIXER_HH
