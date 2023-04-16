#ifndef SCHEDULER_HELPER_HH
#define SCHEDULER_HELPER_HH

#include <cstring>
#include <iostream>
#include <pthread.h>
class Scheduler_helper {
  public:
    static void set_thread_priority(pthread_t handle, std::string const &threadname);
};

#endif // SCHEDULER_HELPER_HH
