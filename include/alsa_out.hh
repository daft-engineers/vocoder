#ifndef ALSA_OUT_HH
#define ALSA_OUT_HH

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
#include "../include/audio/audio.hh"

class alsa_out {
  public:
    alsa_out(const std::string &device_name);
    void run();
    void stop();

  private:

    Pipe<Audio> &input;
    std::thread alsa_out_thread;
    const std::chrono::milliseconds timeout{1};

    snd_pcm_hw_params_t *params{};
    snd_pcm_t *handle{};
    bool running{false};
    snd_pcm_uframes_t frames = 32; // NOLINT(cppcoreguidelines-avoid-magic-numbers) this is modified in place later
};

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#endif // ALSA_OUT_HH