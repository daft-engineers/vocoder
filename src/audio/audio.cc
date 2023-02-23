#include "audio.hh"

class ADCSample : public Audio{
public:
    ADCSample(float sample)
    : Audio(sample){}
};

class FilterOut : public Audio{
    FilterOut(float sample)
    : Audio(sample){}
};

class PowerLevel : public Audio{
    PowerLevel(float sample)
    : Audio(sample){}
};

class AmpOut : public Audio{
    AmpOut(float sample)
    : Audio(sample){}
};

class MixerOut : public Audio{
    MixerOut(float sample)
    : Audio(sample){}
};
