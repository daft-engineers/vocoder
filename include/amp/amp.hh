#ifndef AMP_AMP_HH
#define AMP_AMP_HH

#include "../audio/audio.hh"
#include "../pipe.hh"

class Amplifier {
  public:
    Amplifier(Pipe<Audio> &input_, Pipe<double> &scaling_, Pipe<Audio> &output_, std::chrono::milliseconds timeout_);

    void run();
    void stop();
    static Audio amplify(Audio &sample, double amount);

  private:
    Pipe<Audio> &input;
    Pipe<double> &scaling;
    Pipe<Audio> &output;

    std::thread amplifier_thread;
    bool thread_alive{false};
    const std::chrono::milliseconds timeout;
};

#endif // AMP_AMP_HH
