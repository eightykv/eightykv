#ifndef TouchSensor_h
#define TouchSensor_h

#include "Arduino.h"
#include <MPR121.h>
#include <Wire.h>
#include "Utility.h"

// 12 electrodes -- this needs to be available gloablly
const byte numElectrodes = 12;
const int threshold[12] = {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};

class TouchSensor
{
  public:
    // Constructor
    TouchSensor(int address);
    // To be invoked over and over, updating the values
    int readTouchData();
    // Checks if a particular index is on
    bool getCapOn(int index);
    // Turn off the other caps when a drum is updated
    
  private:
    // Internal sensor object
    MPR121_t cap;

    // Initialization data
    const int touchThreshold = 70;
    const int releaseThreshold = 35;
    const int capSenseInterval = 50;
    const int waitTime = 1500;

    // On/off touch data for drumtree
    bool capOn[12];
    int  elapsed[12];

    // Timer for knowing when to check cap sensor
    long timer;
};

#endif
