/* daft engineers vocoder project
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef PIPE_HH
#define PIPE_HH

#include <condition_variable>
#include <queue>
#include <thread>

/**
 * Pipe struct to allow data to flow between modules in a thread safe way.
 *
 * Data is placed into a pipe by placing it on the back of the pipe's queue. It can then be retrieved from the front of
 * the pipe's queue. The mutex must be locked when interacting with the queue to prevent memory safety issues. With the
 * use of the `wait()` and `notify_all()` functions, a thread can sleep while the queue is empty.
 *
 * The pipe can be used as followed, for example
 * ~~~~~~~~~~~~~~~{.cpp}
 *
 * // declarations
 * Pipe<type> input;
 * Pipe<type> output;
 *
 *
 * std::unique_lock<std::mutex> input_lk(input.cond_m);
 * input_lk.unlock();
 *
 * while(loop) {
 *   input_lk.lock();
 *   wait(input_lk, [this] {return input.queue.empty() == false;})
 *   type data = input.queue.front();
 *   input.queue.pop();
 *   input_lkk.unlock();
 *
 *   // do logic here
 *
 *   {
 *     std::lock_guard<std::mutex> out_lk(output.cond_m);
 *     output.queue.push(audio_out);
 *   }
 *   output.cond.notify_all();
 * }
 *
 * ~~~~~~~~~~~~~~~
 */
template <typename T> struct Pipe {
    std::condition_variable cond; /**< Condition variable for the pipe, used to wait/notify.*/
    std::mutex cond_m;            /**< Mutex for the condition variable.*/
    std::queue<T> queue{};        /**< Queue to hold the data.*/
};

#endif // PIPE_HH
