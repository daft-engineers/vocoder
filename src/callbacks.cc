/* daft engineers vocoder project
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "../include/callbacks.hh"
#include "../include/scheduler_helper.hh"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <sys/types.h>
#include <vector>

alsa_callback::acb::acb(const std::string &device_name) {
    int errorcode = snd_pcm_open(&handle, device_name.c_str(), SND_PCM_STREAM_CAPTURE, 0);
    int dir = 0; // *_near() functions use this to say if the chosen value was above or below the requested value

    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to open pcm device: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast) don't lint problems caused by external libraries
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);

    errorcode = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set access: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

    errorcode = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set format: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

    errorcode = snd_pcm_hw_params_set_channels(handle, params, 2);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set channels: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

    errorcode = snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set rate: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

    errorcode = snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set size: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

#ifndef NDEBUG
    std::cout << "alsa_callback: set sample rate to: " << sample_rate << "Hz\n";
    std::cout << "alsa_callback: set period size to: " << frames << " bytes (channels * samples bytes)\n";

    unsigned int period_time = 0;
    errorcode = snd_pcm_hw_params_get_period_time(params, &period_time, &dir);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to get period time: " << snd_strerror(errorcode) << "\n";
    }
    std::cout << "alsa_callback: period time is: " << period_time << "us\n";
#endif

    errorcode = snd_pcm_hw_params(handle, params);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set hw paramters: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }
}

void alsa_callback::acb::listen(
    const std::function<void(const std::vector<int16_t> &, const std::vector<int16_t> &)> &callback) {
    keep_listening = true;
    cb_thread = std::thread([this, &callback]() {
        while (this->keep_listening) {
            std::vector<int16_t> buffer{};
            buffer.resize(this->frames * 2);

            // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions) error comes from ALSA library
            int errorcode = snd_pcm_readi(this->handle, buffer.data(), this->frames);

            if (errorcode == -EPIPE) {
                std::cerr << "alsa_callback: Overrun occurred\n";
                snd_pcm_prepare(this->handle);
            } else if (errorcode < 0) {
                std::cerr << "alsa_callback: Error from read " << snd_strerror(errorcode);
            } else if (errorcode != (int)this->frames) {
                std::cerr << "alsa_callback: short read, read " << errorcode << " frames\n";
            }

            std::vector<int16_t> modulator{};
            modulator.resize(this->frames);
            std::vector<int16_t> carrier{};
            carrier.resize(this->frames);

            bool toggle = false;
            std::partition_copy(std::begin(buffer), std::end(buffer), std::begin(modulator), std::begin(carrier),
                                [&toggle](int16_t) {
                                    toggle = !toggle;
                                    return toggle;
                                });

            callback(modulator, carrier);
        }
    });

    Scheduler_helper::set_thread_priority(cb_thread.native_handle(), "callbacks");
}

unsigned int alsa_callback::acb::get_sample_rate() {
    return sample_rate;
}

void alsa_callback::acb::stop() {
    keep_listening = false;
    cb_thread.join();
    snd_pcm_drop(handle);
    snd_pcm_close(handle);
}
