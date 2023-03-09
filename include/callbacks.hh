#ifndef CALLBACKS_HH
#define CALLBACKS_HH

/*
Example usage of this functionality:

===================================================

#include "../include/callbacks.hh"
#include <numeric>

int main () {
    // Instantiate by calling the constructor.
    alsa_callback::acb test {};

    // Callback function to be registered with listen class. It must accept a single argument
    // of a std::vector<uint16_t>. This example function calculates the average value of each
    // vector and then prints it out.
    auto audio_callback =
        [](std::vector<uint16_t> vec) {
            std::cout << std::accumulate(vec.begin(), vec.end(), decltype(vec)::value_type(0))/vec.size() << "\n";
        };

    // register callback with listen handler.
    test.listen(audio_callback);
    std::cout << "sleeping main thread for 1 sec" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "sleeping finished" << std::endl;
    test.stop();
}

*/

#include <alsa/asoundlib.h>
#include <alsa/error.h>
#include <alsa/pcm.h>
#include <cstdint>
#include <functional>
#include <iostream>
#include <sys/types.h>
#include <thread>
#include <vector>

namespace alsa_callback {

class acb {
  public:
    acb();
    void listen(const std::function<void(std::vector<uint16_t>)> &callback);
    void stop();

  private:
    std::thread cb_thread;
    snd_pcm_hw_params_t *params{};
    snd_pcm_t *handle{};
    bool keep_listening{false};
    snd_pcm_uframes_t frames = 32; // NOLINT(cppcoreguidelines-avoid-magic-numbers) this is modified in place later
};

} // namespace alsa_callback

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#endif // CALLBACKS_HH
