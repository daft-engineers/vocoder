#include "../include/scheduler_helper.hh"

void Scheduler_helper::set_thread_priority(pthread_t handle, std::string const &threadname) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init) variable is handed in to function to be filled out
    sched_param sch;
    // NOLINTNEXTLINE(cppcoreguidelines-init-variables) variable is handed in to function to be filled out
    int policy;
    pthread_getschedparam(handle, &policy, &sch);
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    sch.sched_priority = 20;
    if (pthread_setschedparam(handle, SCHED_FIFO, &sch)) {
        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        std::cerr << "Failed to set sched param: " << std::strerror(errno) << std::endl;
    } else {
        std::cerr << "Priority increased successfully (" << threadname << ")" << std::endl;
    }
}
