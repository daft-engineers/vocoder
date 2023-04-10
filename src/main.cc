#include "../include/filter/filter.hh"
#include "../include/alsa_out.hh"
#include "../include/callbacks.hh"
#include "../include/rms/rms.hh"
#include "../include/amp/amp.hh"
#include "../include/mixer/mixer.hh"
#include "../include/alsa_out.hh"
#include "../include/pipe.hh"
#include "../include/audio/audio.hh"

int main () {


    alsa_callback::acb alsa_in ("hw:2,0,0");

    const int num_filters = 4;
    std::array<BPFilter, num_filters> carrier_bank{};
    std::array<BPFilter, num_filters> modulator_bank{};
    int width = 10000 / num_filters;
    int centre = width / 2;

    std::array<rms::RMS<1000000>, num_filters> power_meter_bank{};

    std::array<Amplifier, num_filters> amp_bank{};

    std::array<Pipe<Audio>, num_filters> carrier_in_pipes{};
    std::array<Pipe<Audio>, num_filters> modulator_in_pipes{};
    std::array<Pipe<Audio>, num_filters> carrier_out_pipes{};
    std::array<Pipe<Audio>, num_filters> modulator_out_pipes{};

    std::array<Pipe<double>, num_filters> power_out_pipes{};

    std::array<Pipe<Audio>, num_filters> amp_out_pipes{};

    Pipe<Audio> mixer_out_pipe;
    mixer::Mixer<num_filters> mix(amp_out_pipes, mixer_out_pipe);

    AlsaOut alsa_out("hw:2,0,0", mixer_out_pipe);

    // Wire stuff together here
    for (int i = 0; i < num_filters; i++) {
        // Filters
        carrier_bank[i] = BPFilter(2, alsa_in.sample_rate, centre, width, carrier_in_pipes[i], carrier_out_pipes[i]);
        modulator_bank[i] = BPFilter(2, alsa_in.sample_rate, centre, width, modulator_in_pipes[i], modulator_out_pipes[i]);
        centre += width;
        carrier_bank[i].run();
        modulator_bank[i].run();

        // Power Meter
        power_meter_bank[i] = rms::RMS<1000000>(modulator_out_pipes[i], power_out_pipes[i]);
        power_meter_bank[i].run();

        // Amp
        amp_bank[i] = Amplifier(carrier_out_pipes[i], power_out_pipes[i], amp_out_pipes[i]);
        amp_bank[i].run();
    }

    auto cb =
            [num_filters](std::vector<uint16_t> carrier, std::vector<uint16_t> modulator) {
                for (int i = 0; i < num_filters; i++) {
                    Pipe<Audio> &carr_in = carrier_in_pipes[i];
                    {
                        std::lock_guard<std::mutex> lk(carr_in.cond_m);
                        carr_in.queue.push(centre_zero(carrier));
                    }

                    Pipe<Audio> &mod_in = modulator_in_pipes[i];
                    {
                        std::lock_guard<std::mutex> lk(mod_in.cond_m);
                        mod_in.queue.push(centre_zero(carrier));
                    }
                }
            };

    mixer.run();
    alsa_in.listen(cb);
    alsa_in.stop();
}
