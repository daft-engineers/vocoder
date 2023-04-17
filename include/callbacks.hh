#ifndef CALLBACKS_HH
#define CALLBACKS_HH

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

/**
 * alsa callback namespace
 */
namespace alsa_callback {
/**
 * Alsa Callback Class. This reads Audio from the input hardware device and, using a callback will write these
 * to the correct pipes.
 * 
 *  Example usage of this functionality:
 *
 * ===================================================
 * ```cpp
 * #include "../include/callbacks.hh"
 *
 * int main () {
 *    alsa_callback::acb test ("hw:2,0,0");
 *
 *    auto cb =
 *        [](const std::vector<int16_t>& left, const std::vector<int16_t>& right) {
 *            std::cout << left[0] << " : " << right[0] << "\n";
 *        };
 *
 *    std::cerr << "created class" << std::endl;
 *    test.listen(cb);
 *    std::cerr << "sleeping for 1 sec\n";
 *    std::this_thread::sleep_for(std::chrono::seconds(1));
 *    std::cerr << "sleeping finished\n";
 *    test.stop();
 * }
 * ```
 */
    class acb {
    public:
        /**
         * Constructs an ALSA Callback. Initialises the device name.
         * @param device_name A string describing the ALSA device to be used (e.g. "hw:2,0,0").
         */
        acb(const std::string &device_name);
        /**
         * Run function. Opens a thread and continuously reads the hardware device input and triggers a callback.
         */
        void listen(const std::function<void(const std::vector<int16_t> &, const std::vector<int16_t> &)> &callback);
        /**
         * Stop function. Tells the thread to join.
         */
        void stop();
        /**
         * Helper function to tell other objects what the sample rate is.
         */
        unsigned int get_sample_rate();

    private:
        // ALSA input thread.
        std::thread cb_thread;
        // ALSA Hardware Parameters, modified in place by ALSA.
        snd_pcm_hw_params_t *params{};
        // ALSA handle, modified in place by ALSA.
        snd_pcm_t *handle{};
        // Bool to determine if running should continue.
        bool keep_listening{false};
        // Number of frames which ALSA reads/ writes at a time. This is modified in place later by ALSA to close value.
        snd_pcm_uframes_t frames = 128;   // NOLINT(cppcoreguidelines-avoid-magic-numbers) this is modified in place later
        // The suggested sample rate. This is modified in place later by ALSA to close value.
        unsigned int sample_rate = 44100; // NOLINT(cppcoreguidelines-avoid-magic-numbers) this is modified in place later
    };

} // namespace alsa_callback

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#endif // CALLBACKS_HH
