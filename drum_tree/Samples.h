#ifndef Samples_h
#define Samples_h

#include "Arduino.h"
#include <MIDI.h>

class Samples {
  public:
    Samples();
    void sendSample();
    void goBack();
    void reset();
  private:
    int midiNote;
    int baseMidiNote;
};

#endif
