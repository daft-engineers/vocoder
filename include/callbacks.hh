#ifndef CALLBACKS_HH
#define CALLBACKS_HH

/*
Example usage of this functionality:

===================================================

#include "../include/callbacks.hh"

int main () {
    alsa_callback::acb test ("hw:2,0,0");

    auto cb =
        [](const std::vector<int16_t>& left, const std::vector<int16_t>& right) {
            std::cout << left[0] << " : " << right[0] << "\n";
        };

    std::cerr << "created class" << std::endl;
    test.listen(cb);
    std::cerr << "sleeping for 1 sec\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cerr << "sleeping finished\n";
    test.stop();
}

*/

#include <alsa/asoundlib.h>
#include <alsa/error.h>
#include <alsa/pcm.h>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <thread>
#include <vector>

namespace alsa_callback {

class acb {
  public:
    acb(const std::string &device_name);
    void listen(const std::function<void(const std::vector<int16_t> &, const std::vector<int16_t> &)> &callback);
    void stop();
    unsigned int get_sample_rate();

  private:
    std::thread cb_thread;
    snd_pcm_hw_params_t *params{};
    snd_pcm_t *handle{};
    bool keep_listening{false};
    snd_pcm_uframes_t frames = 128;   // NOLINT(cppcoreguidelines-avoid-magic-numbers) this is modified in place later
    unsigned int sample_rate = 44100; // NOLINT(cppcoreguidelines-avoid-magic-numbers) this is modified in place later
};

} // namespace alsa_callback

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#endif // CALLBACKS_HH
