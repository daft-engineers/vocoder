#ifndef FILTER_FILTER_HH
#define FILTER_FILTER_HH

#include "../audio/audio.hh"
#include "../pipe.hh"
#include <Iir.h>
#include <climits>
#include <condition_variable>
#include <thread>

class BPFilter {
  public:
    BPFilter(int order, double sampling_rate, double centre_freq_, double freq_range_, Pipe<Audio> &input_,
             Pipe<Audio> &output_);

    ~BPFilter();

    BPFilter(const BPFilter &) = delete;
    BPFilter &operator=(const BPFilter &) = delete;
    BPFilter(BPFilter &&) = delete;
    BPFilter &operator=(BPFilter &&) = delete;

    void run();
    void stop();
    Audio filter(const Audio &in_audio);

  private:
    static const int default_order{12};
    Iir::Butterworth::BandPass<default_order> f;
    double centre_freq{0};
    double freq_range{UINT_MAX};

    Pipe<Audio> &input;
    Pipe<Audio> &output;

    std::thread filter_thread;
    bool running{false};
    bool thread_alive{false};
};

#endif