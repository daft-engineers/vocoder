#include "../include/alsa_out.hh"
#include "../include/amp.hh"
#include "../include/audio.hh"
#include "../include/callbacks.hh"
#include "../include/filter.hh"
#include "../include/mixer.hh"
#include "../include/pipe.hh"
#include "../include/rms.hh"
#include <cstdlib>
#include <mutex>
#include <ostream>

// Doxygen mainpage
/**
 * \mainpage
 * \section aaa Daft vocoder. 
 * See navigation bar for classes and other documentation.
*/

static void show_usage(const std::string &prog_name) {
    std::cerr << "Usage: " << prog_name << " [-h] [-i input device] [-o output device]\n"
              << "\nOptional arguments:\n"
              << "    -i : The name of the ALSA capture device to use. Defaults to \"hw:2,0,0\"\n"
              << "    -o : The name of the ALSA playback device to use. Defaults to \"hw:2,0,0\"\n"
              << "    -h : Print this usage message\n";
}

int main(int argc, char *argv[]) {
    std::vector<std::string> args{};
    args.reserve(argc);
    for (int i = 0; i < argc; i++) {
        args.emplace_back(argv[i]); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic) it comes from C :(
    }

    std::string input_device_name = "hw:2,0,0";
    std::string output_device_name = "hw:2,0,0";

    auto help_flag = std::find(args.begin(), args.end(), "-h");
    if (help_flag != args.end()) {
        show_usage(args.at(0));
    }

    auto input_flag = std::find(args.begin(), args.end(), "-i");
    if (input_flag != args.end()) {
        input_device_name = *std::next(input_flag);
    }

    auto output_flag = std::find(args.begin(), args.end(), "-o");
    if (output_flag != args.end()) {
        output_device_name = *std::next(output_flag);
    }

    alsa_callback::acb alsa_in(input_device_name);

    const int num_filters = 10;
    const int num_samples = 300;
    const std::chrono::milliseconds timeout(2000);
    std::vector<BPFilter> carrier_bank;
    carrier_bank.reserve(num_filters);
    std::vector<BPFilter> modulator_bank;
    modulator_bank.reserve(num_filters);
    int width = 10000 / num_filters; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    int centre = width / 2;

    std::vector<RMS> power_meter_bank;
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

    AlsaOut alsa_out(output_device_name, mixer_out_pipe, timeout);
    // Wire stuff together here
    for (int i = 0; i < num_filters; i++) {
        // Filters
        carrier_bank.emplace_back(2, alsa_in.get_sample_rate(), centre, width, carrier_in_pipes.at(i),
                                  carrier_out_pipes.at(i), timeout);
        modulator_bank.emplace_back(2, alsa_in.get_sample_rate(), centre, width, modulator_in_pipes.at(i),
                                    modulator_out_pipes.at(i), timeout);
        centre += width;
        carrier_bank.at(i).run();
        modulator_bank.at(i).run();

        // Power Meter
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
        power_meter_bank.emplace_back(num_samples, 10, modulator_out_pipes.at(i), power_out_pipes.at(i), timeout);
        power_meter_bank.at(i).run();

        // Amp
        amp_bank.emplace_back(carrier_out_pipes.at(i), power_out_pipes.at(i), amp_out_pipes.at(i), timeout);
        amp_bank.at(i).run();
    }

    auto cb = [&carrier_in_pipes, &modulator_in_pipes](const std::vector<int16_t> &modulator,
                                                       const std::vector<int16_t> &carrier) {
        for (int i = 0; i < num_filters; i++) {
            Pipe<Audio> &carr_in = carrier_in_pipes.at(i);
            {
                std::lock_guard<std::mutex> lk(carr_in.cond_m);
                carr_in.queue.push(carrier);
            }
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

    alsa_out.stop();
}
