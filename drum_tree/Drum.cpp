#include "Arduino.h"
#include "Drum.h"
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

Drum::Drum(int _index, int _pin, bool _swing, float _randomness, int _midiNote, bool _randomizeDrum) {
  // Set the local params
  pin = _pin;
  index = _index;
  swing = _swing;
  density = -1;
  randomness = _randomness;
  randomizeDrum = _randomizeDrum;
  drumOn = false;
  for (int i = 0; i < 3; i++) {
    densityPins[i] = false;
  }

  if (randomizeDrum) {
    midiNote = _midiNote + random(6);
  }
  else {
    midiNote = _midiNote;
  }
  lastNote = 0;

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
    // Make sure to send a note off for this when we finish
    MIDI.sendNoteOff(midiNote, 127, 7);
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
    densityPins[_density] = true;
    drumIsOn = true;
    probability->setDensity(_density);
  }
  else {
    densityPins[_density] = false;
    _density = -1;
    for (int i = 0; i < 3; i++) {
      if (densityPins[i]) {
        drumIsOn = true;
        _density = i;
      }
    }
    probability->setDensity(_density);
  }
  
  density = _density;
  setDrumOn(drumIsOn);
}

void Drum::setRandomness(float _randomness) {
  randomness = _randomness;
  probability->setRandomness(_randomness);
}

void Drum::sendHit(int i, bool isAlt) {
  i = swing ? i * 2 : i;
  i = i % maxBarLength;
  if (drumOn) {
    bool isHit = isAlt ? altHits[i] : hits[i];
    if (isHit) {
      int tempNote = midiNote;
      if (randomizeDrum) {
        int doRand = random(100);
        if (doRand < randomNoteProb) {
          tempNote = midiNote + random(6);
        }
      }
      //Serial.println("send hit");
      MIDI.sendNoteOff(lastNote, 127, 7);
      MIDI.sendNoteOn(tempNote, 127, 7);
      
      lastNote = tempNote;
      turnOffLED(); // LED is on EXCEPT for the first 1/3rd of a beat
    }
  }
}

void Drum::turnOffLED() {
  digitalWrite(pin, LOW);
}

void Drum::turnOnLED() {
  digitalWrite(pin, HIGH);
}

void Drum::manualHit() {
  // Always turn it off first
  MIDI.sendNoteOff(midiNote, 127, 7);
  MIDI.sendNoteOn(midiNote, 127, 7);
}

void Drum::manualOff() {
  MIDI.sendNoteOff(midiNote, 127, 7);
}

/**
 * This is the most important method
 * Generates a new rhythm using the density arrays
 */
void Drum::newPattern(bool isAlt) {
  
  // Always turn the drum off before starting a new pattern
  //MIDI.sendNoteOff(midiNote, 127, 7);
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






