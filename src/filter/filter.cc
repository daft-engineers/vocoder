#include "filter.hh"

class BPFilter : public Filter {
  public:
    BPFilter(int order, double sampling_rate, double centre_freq_, double freq_range_)
        : Filter(order), centre_freq(centre_freq_), freq_range(freq_range_) {

        f.setup(order, sampling_rate, centre_freq, freq_range);
    }

    Audio filter(Audio in_audio) override {

        Audio filtered_audio;
        for (auto sample : in_audio.sample) {
            filtered_audio.sample.push_back(f.filter(sample));
        }

        return filtered_audio;
    }

  private:
    void call_back() override {
    }

    static const int default_order = 100;
    Iir::Butterworth::BandPass<default_order> f;
    double centre_freq{0};
    double freq_range{UINT_MAX};
    Audio input;
    Audio output;
};
