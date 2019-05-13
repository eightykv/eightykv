#ifndef Samples_h
#define Samples_h

#include "Arduino.h"
#include "Utility.h"

class Samples {
  public:
    Samples();
    void sendSample();
    void goBack();
    void sampleReset();
  private:
    int counter;
};

#endif
