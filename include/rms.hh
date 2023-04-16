/* daft engineers vocoder project
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef RMS_HH
#define RMS_HH

#include "audio.hh"
#include "pipe.hh"
#include "scheduler_helper.hh"
#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>

/** RMS (Root Mean Square) Class.
 *  This class determines the average power of a signal.
 */
class RMS {
  private:
    // Input pipe for the class
    Pipe<Audio> &input_pipe;
    // Output pipe for the class
    Pipe<double> &output_pipe;

    // Ring buffer which contains the recent history of the signal after it has been squared.
    std::vector<uint32_t> squared_sample_buffer = {0};
    // Points to the current head of the buffer
    uint32_t sample_buffer_index = 0;

    // Determines how the long the thread should wait before exiting if no data is received.
    const std::chrono::milliseconds timeout;
    // Boolean variable to track if the thread is running
    bool thread_alive{false};

    // Contains the thread carrying out the calculation
    std::thread thread;

  public:
    /**
     *   Initialises the input and output pipes, and the timeout.
     *   @param num_samples num_samples The number of samples to store in history when calculating the root mean
     *      square. This should be approximately 4x the period of the longest component you want to
     *      include in the output.
     *   @param input_pipe A Pipe<Audio> containing the signal to be measured
     *   @param output_pipe A Pipe<double> which will contain the root mean square of the signal
     *   @param timeout The number of milliseconds the thread should wait before exiting if no data is provided.
     */
    RMS(int num_samples, Pipe<Audio> &input_pipe, Pipe<double> &output_pipe, std::chrono::milliseconds timeout);

    /**
     *   Takes a collection of samples from an Audio item, squares it, and inserts it into
     *   the ring buffer.
     *
     *   @param packet The samples to insert.
     */
    void insert(Audio packet);
    /**
     *   Determines the relative Root Mean Square of the ring buffer. Since the ring buffer already
     *   contains the squares of the samples, this only needs to sum and then square root
     *   the ring buffer.
     *
     *   @return The root mean square (between 0 and 1)
     */
    double calc();

    /**
     *   Creates the calculation thread and then returns.
     */
    void run();

    /**
     *   Stop the thread.
     */
    void stop() {
        if (thread_alive) {
            thread.join();
        }
    }
};

#endif // RMS_HH
