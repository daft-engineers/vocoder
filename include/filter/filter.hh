#ifndef FILTER_H
#define FILTER_H

#include <Iir.h>
#include "../audio/audio.hh"
#include "../pipe.hh"
#include <limits.h>
#include <stdio.h>
#include <thread>
#include <condition_variable>
#include <memory>

class BPFilter{
public:
    BPFilter(int order, double sampling_rate, double centre_freq_, double freq_range_, Pipe<Audio> &input_, Pipe<Audio> &output_);

    void run();
    void stop();
    Audio filter(Audio in_audio);

private:

    static const int default_order{12};
    Iir::Butterworth::BandPass<default_order> f;
    double centre_freq{0};
    double freq_range{UINT_MAX};

    Pipe<Audio> *input;
    Pipe<Audio> *output;

    std::thread filter_thread;
    bool running{false};

};

#endif