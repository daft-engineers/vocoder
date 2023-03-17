#include "amp.hh"

class Amplifer {
    public:

    Audio amplify(Audio sample, float amount){
   Audio processed;
   for (int i = 0; i<sample.sample.size(); i++){
    processed.sample.push_back(sample.sample[i]*amount);
   }
   return processed;
    }
};