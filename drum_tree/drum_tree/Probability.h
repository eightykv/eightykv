#ifndef Probability_h
#define Probability_h

#include "Arduino.h"

// Want this to be available everywhere
const int probabilityArray[5][16] = {
  // Kick
  {80, 20, 40, 20, /*2*/ 40, 10, 40, 20, /*3*/ 75, 20, 40, 20, /*4*/ 30, 30, 30, 30}, 
  // Snare
  {40, 10, 40, 20, /*2*/ 80, 20, 40, 20, /*3*/ 30, 30, 30, 30, /*4*/ 75, 20, 40, 20}, 
  // Hat
  {40, 30, 40, 30, /*2*/ 40, 30, 40, 30, /*3*/ 40, 30, 40, 30, /*4*/ 40, 30, 40, 30}, 
  // Kick 2
  {20, 30, 70, 30, /*2*/ 20, 50, 60, 30, /*3*/ 20, 30, 70, 30, /*4*/ 20, 30, 60, 30}, 
  // Etc
  {10, 30, 10, 20, /*2*/ 20, 30, 40, 30, /*3*/ 30, 30, 30, 30, /*4*/ 10, 20, 10, 40}
};
const byte maxBarLength = 16;

class Probability
{
  public:
    Probability(int index, float randomness);
    void setDensity(int density);
    void setRandomness(float randomness);
    int* getProbability();
  private:
    int baseProbs[16];
    int scaledProbs[16];
    int randomProbs[16];
    int minProb;
    int maxProb;
    float baseMean;
    float scaledMean;
};

#endif
