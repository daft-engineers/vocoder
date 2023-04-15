#ifndef ALSA_OUT_HH
#define ALSA_OUT_HH

#include "audio/audio.hh"
#include "pipe.hh"
#include <alsa/asoundlib.h>
#include <alsa/error.h>
#include <alsa/pcm.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

class AlsaOut {
  public:
    AlsaOut(const std::string &device_name, Pipe<Audio> &input_, std::chrono::milliseconds timeout_);

    void run();
    void stop();

  private:
    Pipe<Audio> &input;
    std::thread alsa_out_thread;
    const std::chrono::milliseconds timeout;

    snd_pcm_hw_params_t *params{};
    snd_pcm_t *handle{};
    snd_pcm_uframes_t frames = 128; // NOLINT(cppcoreguidelines-avoid-magic-numbers) this is modified in place later
};

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#endif // ALSA_OUT_HH