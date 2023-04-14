#include "../include/alsa_out.hh"
#include "../include/amp/amp.hh"
#include "../include/audio/audio.hh"
#include "../include/callbacks.hh"
#include "../include/filter/filter.hh"
#include "../include/mixer/mixer.hh"
#include "../include/pipe.hh"
#include "../include/rms/rms.hh"

int main() {

    alsa_callback::acb alsa_in("hw:2,0,0");

    const int num_filters = 4;
    const std::chrono::milliseconds timeout(100);
    std::vector<BPFilter> carrier_bank;
    std::vector<BPFilter> modulator_bank;
    int width = 10000 / num_filters; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    int centre = width / 2;

    std::vector<rms::RMS<1000000>> power_meter_bank; // NOLINT(cppcoreguidelines-avoid-magic-numbers)

    std::vector<Amplifier> amp_bank;

    std::array<Pipe<Audio>, num_filters> carrier_in_pipes;
    std::array<Pipe<Audio>, num_filters> modulator_in_pipes;
    std::array<Pipe<Audio>, num_filters> carrier_out_pipes;
    std::array<Pipe<Audio>, num_filters> modulator_out_pipes;

    std::array<Pipe<double>, num_filters> power_out_pipes;

    std::array<Pipe<Audio>, num_filters> amp_out_pipes;

    Pipe<Audio> mixer_out_pipe;
    mixer::Mixer<num_filters> mix(amp_out_pipes, mixer_out_pipe);

    AlsaOut alsa_out("hw:2,0,0", mixer_out_pipe);

    // Wire stuff together here
    for (int i = 0; i < num_filters; i++) {
        // Filters
        carrier_bank.emplace_back(2, alsa_in.getSR(), centre, width, carrier_in_pipes.at(i), carrier_out_pipes.at(i));
        modulator_bank.emplace_back(2, alsa_in.getSR(), centre, width, modulator_in_pipes.at(i),
                                    modulator_out_pipes.at(i));
        centre += width;
        carrier_bank.at(i).run();
        modulator_bank.at(i).run();

        // Power Meter
        power_meter_bank.emplace_back(modulator_out_pipes.at(i), power_out_pipes.at(i));
        power_meter_bank.at(i).run();

        // Amp
        amp_bank.emplace_back(carrier_out_pipes.at(i), power_out_pipes.at(i), amp_out_pipes.at(i), timeout);
        amp_bank.at(i).run();
    }

    auto cb = [&carrier_in_pipes, &modulator_in_pipes](std::vector<uint16_t> carrier, std::vector<uint16_t> modulator) {
        for (int i = 0; i < num_filters; i++) {
            Pipe<Audio> &carr_in = carrier_in_pipes.at(i);
            {
                std::lock_guard<std::mutex> lk(carr_in.cond_m);
                carr_in.queue.push(centre_zero(carrier));
            }

            Pipe<Audio> &mod_in = modulator_in_pipes.at(i);
            {
                std::lock_guard<std::mutex> lk(mod_in.cond_m);
                mod_in.queue.push(centre_zero(modulator));
            }
        }
    };

    mix.run();
    alsa_in.listen(cb);
    //alsa_in.stop();
}
