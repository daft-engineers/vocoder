#ifndef FILTER_H
#define FILTER_H

#include <Iir.h>
#include "../audio/audio.hh"
#include "../pipe.hh"
#include <limits.h>
#include <stdio.h>
#include <thread>
#include <condition_variable>
#include <memory>

class Filter
{
public:
    Filter(unsigned int order_)
            : order(order_){
        if (order_ < 1){
            printf("Invalid filter parameters.");
            std::exit(-1);
        }
    }

    virtual ~Filter() = default;

    //placeholder
    int setup(){
        return order;
    }

    virtual void run() = 0;


private:

    virtual Audio filter(Audio in_audio) = 0;

    //IIr::Butterworth::BandPass<order> f;
    //int num_of_taps;
    unsigned int order;
};

#endif
