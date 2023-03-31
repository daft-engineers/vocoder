#ifndef AMP_AMP_HH
#define AMP_AMP_HH

#include "../audio/audio.hh"
#include "../pipe.hh"

class Amplifier {
  public:
    Amplifier(Pipe<Audio> &input_, Pipe<double> &scaling_, Pipe<Audio> &output_);
    ~Amplifier();
    Amplifier(const Amplifier &) = delete;
    Amplifier &operator=(const Amplifier &) = delete;
    Amplifier(Amplifier &&) = delete;
    Amplifier &operator=(Amplifier &&) = delete;

    void run();
    Audio amplify(Audio &sample, double amount);

  private:
    Pipe<Audio> &input;
    Pipe<double> &scaling;
    Pipe<Audio> &output;

    std::thread amplifier_thread;
    bool thread_alive{false};
    const std::chrono::milliseconds timeout{1};
};

#endif // AMP_AMP_HH
