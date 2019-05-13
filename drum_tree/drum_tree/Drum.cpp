#include "Arduino.h"
#include "Drum.h"

Drum::Drum(int _index, int _pin, bool _swing, float _randomness) {
  // Set the local params
  pin = _pin;
  index = _index;
  swing = _swing;
  density = -1;
  randomness = _randomness;
  drumOn = false;
  for (int i = 0; i < 3; i++) {
    densityPins[i] = false;
  }
  
  probability = new Probability(_index, _randomness);

  for(int i = 0; i < maxBarLength; i++) {
    hits[i] = false;
    altHits[i] = false;
  }

  //turnOnLED();
}

bool Drum::getDrumOn() {
  return drumOn;
}
void Drum::setDrumOn(bool _drumOn) {
  //Serial.println("SetDrumOn");
  
  drumOn = _drumOn;
  if (drumOn == true) {
    // Starting up!
    newPattern(false);
  }
  else {
    // Something??
  }

  digitalWrite(pin, drumOn ? HIGH : LOW);
}

int Drum::getDensity() {
  return density;
}
void Drum::setDensity(int _density, bool capOn) {
  bool drumIsOn = false;
  if (capOn) {
    density = _density;
    drumIsOn = true;
  }
  else {
    density = -1;
  }

  probability->setDensity(density);
  setDrumOn(drumIsOn);
}

void Drum::setRandomness(float _randomness) {
  randomness = _randomness;
  probability->setRandomness(_randomness);
}

int Drum::getHit(int i, bool isAlt) {
  i = swing ? i * 2 : i;
  i = i % maxBarLength;
  if (drumOn) {
    bool isHit = isAlt ? altHits[i] : hits[i];
    if (isHit) {
      turnOffLED(); // LED is on EXCEPT for the first 1/3rd of a beat
      return 1;
    }
    else {
      return 0;
    }
  }
}

void Drum::turnOffLED() {
  digitalWrite(pin, LOW);
}

void Drum::turnOnLED() {
  digitalWrite(pin, HIGH);
}

/**
 * This is the most important method
 * Generates a new rhythm using the density arrays
 */
void Drum::newPattern(bool isAlt) {
  
  // Always turn the drum off before starting a new pattern
  int *probabilityRow = probability->getProbability();

  for(int i = 0; i < maxBarLength; i++) {
    bool hit = false;

    // If density is -1, no hits at all
    if (density != -1) {
      // Generate a random number between 0 and 100
      int r = random(100);
      // If the number is less than the value at this location, it's a hit!
      if (r < probabilityRow[i]) {
        hit = true;
      }
    }
    
    if (isAlt) {
      altHits[i] = hit;
    }
    else {
      hits[i] = hit;
    }
  }
}
