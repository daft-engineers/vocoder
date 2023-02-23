#ifndef VOCODER_AUDIO_HH
#define VOCODER_AUDIO_HH

class Audio {
public:
    Audio(float sample_)
    : sample(sample_){}

private:
    float sample;
};


#endif //VOCODER_AUDIO_HH
