#include "../include/amp/amp.hh"

Audio Amplifer::amplify(Audio sample, float amount){
   Audio processed;
   for (int i = 0; i<sample.size(); i++){
    processed.push_back(sample[i]*amount);
   }
   return processed;
    }
