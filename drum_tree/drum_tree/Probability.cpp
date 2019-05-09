#include "Arduino.h"
#include "Probability.h"

Probability::Probability(int index, float randomness) {
  baseMean = 0.0;
  minProb = 100;
  maxProb = 0;
  for (int i = 0; i < maxBarLength; i++) {
    randomProbs[i] = scaledProbs[i] = baseProbs[i] = probabilityArray[index][i];
    baseMean += baseProbs[i];
    if (baseProbs[i] < minProb) minProb = baseProbs[i];
    if (baseProbs[i] > maxProb) maxProb = baseProbs[i];
  }
  baseMean = baseMean / 16.0;
  scaledMean = baseMean;

  setRandomness(randomness);
}

void Probability::setDensity(int density) {
  // If density is -1, this drum is off; everything is 0
  if (density == -1) {
    for (int i = 0; i < maxBarLength; i++) {
      scaledProbs[i] = 0;
    }
    scaledMean = 0;
    return;
  }
  
  // Scale by the baseMean divided by 2
  int scale = 0;
  if (density != 1) {
    scale = baseMean * (density > 1 ? 1 : -0.67);
  }

  scaledMean = 0.0;
  // Scale each one -- never goes below 1, never above 99
  for (int i = 0; i < maxBarLength; i++) {
    int thisScale = baseProbs[i] + scale;
    thisScale = thisScale > 98 ? 98 : thisScale;
    thisScale = thisScale < 5 ? 5 : thisScale;
    scaledProbs[i] = thisScale;
    scaledMean += scaledProbs[i];
    randomProbs[i] = scaledProbs[i];
  }
  
  scaledMean = (scaledMean / 16.0);
  scaledMean = scaledMean + (scaledMean/2);
}

void Probability::setRandomness(float randomness) {
  // Bring each of them closer to the baseMean
  for (int i = 0; i < maxBarLength; i++) {
    int diff = scaledMean - scaledProbs[i];
    float thisRandom = scaledProbs[i] + (diff * randomness);
    randomProbs[i] = ceil(thisRandom);
  }
}

int *Probability::getProbability() {
  return randomProbs;
}

