#ifndef MIXER_MIXER_HH
#define MIXER_MIXER_HH

#include "../audio/audio.hh"
#include "../pipe.hh"
#include <array>
#include <cstddef>
#include <functional>

namespace mixer {

template <std::size_t num_banks> class Mixer {
  private:
    std::array<Pipe<Audio>, num_banks> *input_pipes;
    Pipe<Audio> *output_pipe;

  public:
    Mixer(std::array<Pipe<Audio>, num_banks> &input_pipes, Pipe<Audio> &output_pipe)
        : input_pipes(&input_pipes), output_pipe(&output_pipe) {
    }

    void run() {
    }
};

} // namespace mixer

#endif // MIXER_MIXER_HH
