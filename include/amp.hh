#ifndef AMP_HH
#define AMP_HH

#include "audio.hh"
#include "pipe.hh"

class Amplifier {
  public:
    Amplifier(Pipe<Audio> &input_, Pipe<double> &scaling_, Pipe<Audio> &output_, std::chrono::milliseconds timeout_);
    ~Amplifier();
    Amplifier(const Amplifier &) = delete;
    Amplifier &operator=(const Amplifier &) = delete;
    Amplifier(Amplifier &&) = delete;
    Amplifier &operator=(Amplifier &&) = delete;

    void run();
    static Audio amplify(Audio &sample, double amount);

  private:
    Pipe<Audio> &input;
    Pipe<double> &scaling;
    Pipe<Audio> &output;

    std::thread amplifier_thread;
    bool thread_alive{false};
    const std::chrono::milliseconds timeout;
};

#endif // AMP_HH
