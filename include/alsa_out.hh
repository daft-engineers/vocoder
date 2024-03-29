/* daft engineers vocoder project
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef ALSA_OUT_HH
#define ALSA_OUT_HH

#include "audio.hh"
#include "pipe.hh"
#include <alsa/asoundlib.h>
#include <alsa/error.h>
#include <alsa/pcm.h>
#include <cstdint>
#include <iostream>
#include <thread>

/**
 * AlsaOut Class. This takes an input pipe containing Audio and writes this to the devices audio output.
 */
class AlsaOut {
  public:
    /**
     * Constructs a ALSA output. Initialises the device name, input pipe and timeout.
     * @param device_name A string describing the ALSA device to be used (e.g. "hw:2,0,0").
     * @param input_ Input pipe.
     * @param timeout_ Time in milliseconds the thread will wait before exiting if no data is provided.
     */
    AlsaOut(const std::string &device_name, Pipe<Audio> &input_, std::chrono::milliseconds timeout_);
    /**
     * Run function. Opens a thread and continuously writes to the output.
     * Runs until stop() is called.
     * @see stop()
     */
    void run();
    /**
     * Stop function. Tells the thread to join.
     * @see run()
     */
    void stop();

  private:
    // Input pipe.
    Pipe<Audio> &input;
    // ALSA output thread.
    std::thread alsa_out_thread;
    // Timeout value for ALSA output thread.
    const std::chrono::milliseconds timeout;

    // ALSA Hardware Parameters, modified in place by ALSA.
    snd_pcm_hw_params_t *params{};
    // ALSA handle, modified in place by ALSA.
    snd_pcm_t *handle{};
    // Number of frames which ALSA reads/ writes at a time. This is modified in place later by ALSA to close value.
    snd_pcm_uframes_t frames = 128; // NOLINT(cppcoreguidelines-avoid-magic-numbers) this is modified in place later
};

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#endif // ALSA_OUT_HH
