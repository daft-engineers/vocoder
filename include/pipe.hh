#ifndef PIPE_HH
#define PIPE_HH

#include <condition_variable>
#include <queue>
#include <thread>

template <typename T> struct Pipe {
    std::condition_variable cond;
    std::mutex cond_m;
    std::queue<T> queue{};
};

#endif // PIPE_HH
