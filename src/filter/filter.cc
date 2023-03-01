#include "filter.hh"

class BPFilter : public Filter {
  public:
    BPFilter(unsigned int order, unsigned int centre_freq_, unsigned int freq_range_)
        : Filter(order), centre_freq(centre_freq_), freq_range(freq_range_) {

        f.setup(order, centre_freq, freq_range);
    }

    Audio filter(Audio in_audio) override {

        std::vector<float> filtered_samples;
        filtered_samples.reserve(in_audio.sample.size());

        for (auto sample : in_audio.sample) {
            filtered_samples.push_back(f.filter(sample));
        }

        Audio filtered_audio;
        filtered_audio.sample = filtered_samples;
        return filtered_audio;
    }

  private:
    void call_back() override {
    }

    Iir::Butterworth::BandPass<2> f;
    unsigned int centre_freq{0};
    unsigned int freq_range{UINT_MAX};
    Audio input;
    Audio output;
};
