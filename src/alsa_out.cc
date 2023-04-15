#include "../include/alsa_out.hh"

AlsaOut::AlsaOut(const std::string &device_name, Pipe<Audio> &input_) : input(input_) {
    // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions) error comes from ALSA library
    int errorcode = snd_pcm_open(&handle, device_name.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    int dir = 0; // *_near() functions use this to say if the chosen value was above or below the requested value
    unsigned int sample_rate = 44100; // NOLINT(cppcoreguidelines-avoid-magic-numbers) this is modified in place later

    if (errorcode < 0) {
        std::cerr << "alsa_out: unable to open pcm device: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

    /* Allocate a hardware parameters object. */
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast) don't lint problems caused by external libraries
    snd_pcm_hw_params_alloca(&params);
    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */

    /* Interleaved mode */
    errorcode = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (errorcode < 0) {
        std::cerr << "alsa_out: unable to set access: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

    /* Signed 16-bit little-endian format */
    errorcode = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    if (errorcode < 0) {
        std::cerr << "alsa_out: unable to set format: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

    /* Two channels (stereo) */
    errorcode = snd_pcm_hw_params_set_channels(handle, params, 2);
    if (errorcode < 0) {
        std::cerr << "alsa_out: unable to set channels: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

    /* 44100 bits/second sampling rate (CD quality) */
    errorcode = snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);
    if (errorcode < 0) {
        std::cerr << "alsa_out: unable to set rate: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

    /* Set period size to 32 frames. */
    errorcode = snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);
    if (errorcode < 0) {
        std::cerr << "alsa_out: unable to set size: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }

#ifndef NDEBUG
    std::cout << "alsa_out: set sample rate to: " << sample_rate << "Hz\n";
    std::cout << "alsa_out: set period size to: " << frames << " bytes (channels * samples bytes)\n";

    unsigned int period_time = 0;
    errorcode = snd_pcm_hw_params_get_period_time(params, &period_time, &dir);
    if (errorcode < 0) {
        std::cerr << "alsa_out: unable to get period time: " << snd_strerror(errorcode) << "\n";
    }
    std::cout << "alsa_out: period time is: " << period_time << "us\n";
#endif

    errorcode = snd_pcm_hw_params(handle, params);
    if (errorcode < 0) {
        std::cerr << "alsa_out: unable to set hw paramters: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    }
}

void AlsaOut::stop() {
    alsa_out_thread.join();
}

void AlsaOut::run() {
    alsa_out_thread = std::thread([this]() {
        std::unique_lock<std::mutex> lk(input.cond_m);
        lk.unlock();

        while (true) {
            lk.lock();
            if (!input.cond.wait_for(lk, timeout, [this] { return input.queue.empty() == false; })) {
                return false;
            }

            // Output audio
            Audio audio_out = input.queue.front();
            // std::cerr << input.queue.size() << std::endl;
            input.queue.pop();
            lk.unlock();
            Audio new_audio;
            for (auto sample : audio_out) {
                new_audio.push_back(sample);
                new_audio.push_back(sample);
            }
            // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions)
            int rc = snd_pcm_writei(handle, new_audio.data(), frames);
            if (rc == -EPIPE) {
                /* EPIPE means underrun */
                std::cerr << "underrun occurred\n";
                snd_pcm_prepare(handle);
            } else if (rc < 0) {
                std::cerr << "error from writei: " << snd_strerror(rc) << "\n";
            } else if (rc != (int)frames) {
                std::cerr << "short write, write " << rc << "\n";
            }
        }
    });
}