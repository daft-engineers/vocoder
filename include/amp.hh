#ifndef AMP_HH
#define AMP_HH

#include "audio.hh"
#include "pipe.hh"

/**
 * Amplifier Class. This takes two input pipes, one containing Audio and one containing a double scaling value.
 * This will output to a pipe containing Audio which is the input audio scaled by the scaling value.
 */
class Amplifier {
  public:
    /**
     * Constructs an Amplifier. Initialises the input Audio pipe, input scaling pipe, output Audio pipe and timeout.
     * @param input_ Input Audio pipe.
     * @param scaling_ Input Scaling pipe.
     * @param output_ Output Audio pipe.
     * @param timeout_ Time in milliseconds the thread will wait before exiting if no data is provided.
     */
    Amplifier(Pipe<Audio> &input_, Pipe<double> &scaling_, Pipe<Audio> &output_, std::chrono::milliseconds timeout_);

    /**
     *  Requests for the calculation thread to join.
     * @see run()
     */
    void stop();

    /**
     * Run function. Opens a thread and continuously scales input audio by scaling, outputting the scaled audio.
     * Runs until stop() is called.
     * @see stop()
     */
    void run();
    /**
     * Amplify helper function. Scales a sample by scale value.
     * @param sample The audio sample which is being scaled.
     * @param amount The scale value to multiply the sample by.
     * @return Audio object containg amplified audio.
     */
    static Audio amplify(Audio &sample, double amount);

  private:
    // Input Audio pipe.
    Pipe<Audio> &input;
    // Input Scaling pipe.
    Pipe<double> &scaling;
    // Output Audio pipe.
    Pipe<Audio> &output;

    // Amplifier runtime thread.
    std::thread amplifier_thread;
    // Boolean to determine if thread is alive for unit testing helper function.
    bool thread_alive{false};
    // Timeout value for Amplifier thread.
    const std::chrono::milliseconds timeout;
};

#endif // AMP_HH
