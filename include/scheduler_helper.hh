#ifndef SCHEDULER_HELPER_HH
#define SCHEDULER_HELPER_HH

#include <cstring>
#include <iostream>
#include <pthread.h>

/**
 * Scheduler helper Class. Holds the helper function
 */
class Scheduler_helper {
  public:
    /**
     * Helper function to set the thread priority. Avoids repeated code.
     *
     * @param handle Thread handle of the thread that needs its priority increased.
     * @param threadname String to pass to debug outputs to see which thread failed
     */
    static void set_thread_priority(pthread_t handle, std::string const &threadname);
};

#endif // SCHEDULER_HELPER_HH
