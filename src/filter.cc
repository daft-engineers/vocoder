#include "../include/filter/filter.hh"

class BPFilter : public Filter {
  public:
    BPFilter(int order, double sampling_rate, double centre_freq_, double freq_range_, Pipe<Audio> &input_, Pipe<Audio> &output_)
        : Filter(order), centre_freq(centre_freq_), freq_range(freq_range_), input(&input_), output(&output_) {

        f.setup(order, sampling_rate, centre_freq, freq_range);
    }

    void run() override {
        filter_thread = std::thread([this]() {
            std::unique_lock<std::mutex> lk(input->cond_m);
            running = true;
            while (running) {
                input->cond.wait(lk, [this] { return input->queue.empty() == false; });

                Audio audio_out = filter(input->queue.front());
                input->queue.pop();

                {
                    std::lock_guard<std::mutex> out_lk(output->cond_m);
                    output->queue.push(audio_out);
                }
                output->cond.notify_all();
            }
        });
    }

    void stop() {
        running = false;
        filter_thread.join();
    }

  private:
    Audio filter(Audio in_audio) override {

        Audio filtered_audio;
        for (auto sample : in_audio) {
            filtered_audio.push_back(f.filter(sample));
        }

        return filtered_audio;
    }

    static const int default_order = 100;
    Iir::Butterworth::BandPass<default_order> f;
    double centre_freq{0};
    double freq_range{UINT_MAX};

    Pipe<Audio> *input;
    Pipe<Audio> *output;

    std::thread filter_thread;
    bool running{false};

};
