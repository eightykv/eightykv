#ifndef Drum_h
#define Drum_h

#include "Arduino.h"
#include "Probability.h"
#include "Utility.h"
const int randomNoteProb = 20;

class Drum
{
  public:
    Drum(int _index, int _pin, bool _swing, float _randomness);
    bool  getDrumOn();
    void  setDrumOn(bool _drumOn);
    int   getDensity();
    void  setDensity(int _density, bool capOn);
    void  setRandomness(float _randomness);
    void  newPattern(bool isAlt);
    int   getHit(int index, bool isAlt);
    void  turnOffLED();
    void  turnOnLED();
  private:
    // Properties
    bool  drumOn;
    bool  hits[16];
    bool  altHits[16];
    bool  densityPins[3];
    int   lastNote;
    int   pin;
    bool  swing;
    int   index;
    int   density;
    float randomness;
    Probability *probability;
};

#endif
