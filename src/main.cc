#include "../include/alsa_out.hh"
#include "../include/amp/amp.hh"
#include "../include/audio/audio.hh"
#include "../include/callbacks.hh"
#include "../include/filter/filter.hh"
#include "../include/mixer/mixer.hh"
#include "../include/pipe.hh"
#include "../include/rms/rms.hh"
#include <mutex>
#include <sys/resource.h>

int main() {

    alsa_callback::acb alsa_in("hw:2,0,0");

    const int num_filters = 10;
    const std::chrono::milliseconds timeout(2000);
    std::vector<BPFilter> carrier_bank;
    carrier_bank.reserve(num_filters);
    std::vector<BPFilter> modulator_bank;
    modulator_bank.reserve(num_filters);
    int width = 10000 / num_filters; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    int centre = width / 2;

    std::vector<rms::RMS<10000>> power_meter_bank; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    power_meter_bank.reserve(num_filters);

    std::vector<Amplifier> amp_bank;
    amp_bank.reserve(num_filters);

    std::array<Pipe<Audio>, num_filters> carrier_in_pipes;
    std::array<Pipe<Audio>, num_filters> modulator_in_pipes;
    std::array<Pipe<Audio>, num_filters> carrier_out_pipes;
    std::array<Pipe<Audio>, num_filters> modulator_out_pipes;

    std::array<Pipe<double>, num_filters> power_out_pipes;

    std::array<Pipe<Audio>, num_filters> amp_out_pipes;

    Pipe<Audio> mixer_out_pipe;
    mixer::Mixer<num_filters> mix(amp_out_pipes, mixer_out_pipe, timeout);

    AlsaOut alsa_out("hw:2,0,0", mixer_out_pipe, timeout);
    // Wire stuff together here
    for (int i = 0; i < num_filters; i++) {
        // Filters
        carrier_bank.emplace_back(2, alsa_in.getSR(), centre, width, carrier_in_pipes.at(i), carrier_out_pipes.at(i), timeout);
        modulator_bank.emplace_back(2, alsa_in.getSR(), centre, width, modulator_in_pipes.at(i),
                                    modulator_out_pipes.at(i), timeout);
        centre += width;
        carrier_bank.at(i).run();
        modulator_bank.at(i).run();

        // Power Meter
        power_meter_bank.emplace_back(modulator_out_pipes.at(i), power_out_pipes.at(i), timeout);
        power_meter_bank.at(i).run();

        // Amp
        amp_bank.emplace_back(carrier_out_pipes.at(i), power_out_pipes.at(i), amp_out_pipes.at(i), timeout);
        amp_bank.at(i).run();
    }

    auto cb = [&carrier_in_pipes, &modulator_in_pipes](const std::vector<int16_t>& modulator, const std::vector<int16_t>& carrier) {
        for (int i = 0; i < num_filters; i++) {
            Pipe<Audio> &carr_in = carrier_in_pipes.at(i);
            //std::cerr << carrier[0] << std::endl;
            {
                std::lock_guard<std::mutex> lk(carr_in.cond_m);
                carr_in.queue.push(carrier);
            }
            // std::cerr << carr_in.queue.empty() << " and " << i << std::endl;
            carr_in.cond.notify_all();

            Pipe<Audio> &mod_in = modulator_in_pipes.at(i);
            {
                std::lock_guard<std::mutex> lk(mod_in.cond_m);
                mod_in.queue.push(modulator);
            }
            mod_in.cond.notify_all();
        }
    };

    mix.run();
    alsa_in.listen(cb);
    alsa_out.run();

    while (true) {};
}

// int main() {
//     alsa_callback::acb test ("hw:2,0,0");
//     Pipe<Audio> pipe;

//     AlsaOut alsa_out ("hw:2,0,0", pipe);

//     auto cb =
//         [&pipe](std::vector<int16_t> left, std::vector<int16_t> right) {
//             //std::cout << left[0] << " : " << right[0] << "\n";
//             {
//                 std::lock_guard<std::mutex> lk(pipe.cond_m);
//                 // Audio centred = centre_zero(left);
//                 // centre_zero(right);
//                 pipe.queue.push(left);
//                 //std::cerr << left[0] << std::endl;
//             }
//         };

//     std::cerr << "created class" << std::endl;
//     alsa_out.run();
//     test.listen(cb);
//     //std::cerr << "sleeping for 1 sec\n";
//     std::this_thread::sleep_for(std::chrono::seconds(60));
//     //std::cerr << "sleeping finished\n";
//     test.stop();
//     alsa_out.stop();
// }
