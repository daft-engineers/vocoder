#ifndef VOCODER_AUDIO_HH
#define VOCODER_AUDIO_HH

class Audio {
  public:
    Audio(std::vector<float> sample_) : sample(sample_) {
    }

  private:
    std::vector<float> sample;
};

#endif // VOCODER_AUDIO_HH
