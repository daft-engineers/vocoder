#ifndef FILTER_H
#define FILTER_H

#include <Iir.h>
#include "../audio/audio.hh"
#include <limits.h>
#include <stdio.h>

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

    virtual Audio filter(Audio in_audio) = 0;

private:
    virtual void call_back() = 0;

    //IIr::Butterworth::BandPass<order> f;
    //int num_of_taps;
    unsigned int order;
};

#endif