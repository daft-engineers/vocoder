#ifndef FILTER_HH
#define FILTER_HH

#include "audio.hh"
#include "pipe.hh"
#include <Iir.h>
#include <climits>
#include <condition_variable>
#include <thread>

/** BandPass Filter Class.
 *  The class for the Band Pass IIR filter, based on [Bernd Porr's IIR implementation](https://github.com/berndporr/iir1). 
 */
class BPFilter {
  public:
    /**
     * Bandpass constructor.
     * Sets up the filter just so.
     * @param order the integer order for the filter
     * @param sampling_rate the sampling rate for the filter
     * @param centre_freq_ the centre freqency for the pass band
     * @param freq_range_ the width of the pass band
     * @param input_ the input pipe for the module
     * @param output_ the output pipe for the module
     * @param timeout_ the timeout for the thread wait
     */
    BPFilter(int order, double sampling_rate, double centre_freq_, double freq_range_, Pipe<Audio> &input_,
             Pipe<Audio> &output_, std::chrono::milliseconds timeout_);

    /**
     * Stop thread
     * Requests the thread to join.
     * @see run()
     */
    void stop();

    /**
     * Run thread function
     * Contains the main logic for the thread. Waits for input, filters the data, and then sends to the output.
     * Runs until stop() is called.
     * @see stop()
     */
    void run();

    /**
     * Filter function
     * Processes each of the samples in the audio stream.
     */
    Audio filter(const Audio &in_audio);

  private:
    /**
     * Default order for the IIR filter used for contruction
     */
    static const int default_order{12};
    /**
     * The IIR filter
     */
    Iir::Butterworth::BandPass<default_order> f;
    /**
     * Centre frequency for the pass band for the filter
     */
    double centre_freq{0};
    /**
     * Frequency range for the pass band of the filter
     */
    double freq_range{UINT_MAX};

    /**
     * Input pipe for the module
     */
    Pipe<Audio> &input;
    /**
     * Output pipe for the module
     */
    Pipe<Audio> &output;

    /**
     * The thread for this instance of filter
     */
    std::thread filter_thread;

    /**
     * Thread alive flag. Holdes state of thread so it is only joined if it is running.
     */
    bool thread_alive{false};

    /**
     * Timeout for the thread wait()
     */
    const std::chrono::milliseconds timeout;
};

#endif
