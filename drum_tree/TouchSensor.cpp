#include "Arduino.h"
#include "TouchSensor.h"

// Constructor
TouchSensor::TouchSensor(int address) {
  // Initialize the capacitor
  cap = MPR121_t();

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (Utility::debug) {
    Serial.print("Looking for address ");
    Serial.println(address, HEX);
  }
  if (!(cap).begin(address)) {
    // Turn on an error 
  } else {
    Serial.print("MPR121_t found at address ");
    Serial.println(address, HEX);
  }
  if (Utility::debug) {
    Serial.println();
  }

  cap.setRegister(MPR121_AFE1, 0xFF); //1111 1111
  cap.setRegister(MPR121_AFE2, 0x30); //1111 0000
  cap.setRegister(MPR121_NHDF, 0x01); //noise half delta (falling)
  cap.setRegister(MPR121_FDLF, 0xFF); //filter delay limit (falling)
  
  cap.setTouchThreshold(touchThreshold);
  cap.setReleaseThreshold(releaseThreshold);

  // Initialize the arrays
  for (int i = 0; i < numElectrodes; i++) {
    capOn[i] = false;
    elapsed[i] = 0;
  }

  timer = 0;
}

// To be invoked over and over, updating the values
int TouchSensor::readTouchData() {
  int changed = -1;
  int baseline, filtered;

  if (millis() - timer > capSenseInterval) {
    timer = millis();
    cap.updateAll();

    for (int i = 0; i < numElectrodes; i++) {
      bool touch = false;
      baseline = cap.getBaselineData(i);
      filtered = cap.getFilteredData(i);
      /*Serial.print(baseline);
      Serial.print(" ");
      Serial.print(filtered);
      Serial.print(" ");*/
      //Serial.print(abs(baseline - filtered));
      //Serial.print(" ");
      if (abs(baseline - filtered) > threshold[i]) {
        touch = true;
      }
      
      if (touch && elapsed[i] > waitTime) {
        elapsed[i] = 0;
        capOn[i] = !capOn[i];
        changed = i;
      }
      else if (elapsed[i] <= waitTime) {
        elapsed[i] = elapsed[i] + 50;
      }
    }
    //Serial.println();
  }

  // Returns whether anything has changed
  return changed;
}

// Checks if a particular index is on
bool TouchSensor::getCapOn(int index) {
  return capOn[index];
}

