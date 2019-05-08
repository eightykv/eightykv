#ifndef Utility_h
#define Utility_h

#include "Arduino.h"

class Utility {
  public:
    const static bool debug = true;
    static Utility* getUtility();
  private:
    static Utility* utility;
    Utility();
};

#endif
