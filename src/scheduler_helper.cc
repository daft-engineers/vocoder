#include "../include/scheduler_helper.hh"

void Scheduler_helper::set_thread_priority(pthread_t handle, std::string const &threadname) {

    sched_param sch{0};
    int policy = 0;
    pthread_getschedparam(handle, &policy, &sch);
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    sch.sched_priority = 20;
    if (pthread_setschedparam(handle, SCHED_FIFO, &sch)) {
        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        std::cerr << "Failed to set sched param: " << std::strerror(errno) << std::endl;
        // note: thread should continue to run after without priority set.
        // Low priority should still work but may have a degraded experience.
    } else {
#ifndef NDEBUG
        std::cerr << "Priority increased successfully (" << threadname << ")" << std::endl;
#endif // NDEBUG
    }
}
