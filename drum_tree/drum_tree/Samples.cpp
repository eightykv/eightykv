#include "Samples.h"

Samples::Samples() {
  counter = 0;
}

void Samples::sendSample() {
  if (Utility::debug) {
    Serial.print("Samples: Sending sample ");
  }
  Serial.println(counter);
  counter++;
}

void Samples::goBack() {
  if (Utility::debug) {
    Serial.println("Samples: Going back");
  }
  if (counter > 0) {
    counter--;
  }
}

void Samples::sampleReset() {
  if (Utility::debug) {
    Serial.println("Samples: Resetting");
  }
  //counter = 0;
}
