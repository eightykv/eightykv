#ifndef TouchSensor_h
#define TouchSensor_h

#include "Arduino.h"
#include <MPR121.h>
#include <Wire.h>
#include "Utility.h"

// 12 electrodes -- this needs to be available gloablly
const byte numElectrodes = 12;
const int threshold = 30;

class TouchSensor
{
  public:
    // Constructor
    TouchSensor(int _address);
    // To be invoked over and over, updating the values
    int readTouchData();
    // Checks if a particular index is on
    bool getElectrodeOn(int index, int drumIndex);
    // Turn off the other caps when a drum is updated
    
  private:
    // Internal sensor object
    MPR121_t cap;

    // Store address
    int address;

    // Initialization data
    const int touchThreshold = 70;
    const int releaseThreshold = 35;
    const int capSenseInterval = 50;
    const int waitTime = 1500;
    const int padWait = 400;
    const int minTempo = 200;

    // On/off touch data for drumtree
    bool electrodeOn[12];
    int  elapsed[12];

    // Timer for knowing when to check cap sensor
    long timer;
};

#endif
