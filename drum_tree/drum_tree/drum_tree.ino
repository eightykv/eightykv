#include "Drum.h"
#include "TouchSensor.h"
#include "Utility.h"
#include "Samples.h"

// 2 touch sensors, 5 drums
const int numCaps = 2;
const int numDrums = 5;
TouchSensor *caps[2];
Drum *drums[5];

Samples *samples;

// Configuration
const int swingPin = 11;
const int tempoPin = A3;
const int randPin = A0;
const int stopIndicatorPin = 51;
const int onIndicatorPin = 52;
const int swingIndicatorPin = 53;
const int randIndicatorPin = 2;
bool drumsOn;
bool swing;
float randomness;

// Tempo
const int tempoThreshold = 500;
const int maxWait = 4000;
float tempo;
bool isLongBeat = true;
int prepareToChange = 0;
float totalTempo = 600.0;
float swingTotalTempo = 600.0;
float longBeat = 0.0;
float shortBeat = 0.0;
int tempoCount = 0;
float diff = 0.0;
long lastKnock;
int minTempo = 300;

// Progression
byte barLength;
long timer;
byte location[2] = {0, 0};
int changeProb[5] = {0, 0, 0, 0, 0};
bool isAlt;

// Output
int d[5];
char output[9];

/****************
 * 
 *  SETUP
 *  Step one: 
 *
 ****************/

void setup() {
  Serial.begin(57600);
  
  while (!Serial) {; // wait for serial port to connect. Needed for native USB port only
  }
  //Serial.println("begin initialize");

  caps[0] = new TouchSensor(0x5A);
  caps[1] = new TouchSensor(0x5B);

  // Setup the pins
  pinMode(swingPin, INPUT);
  pinMode(tempoPin, INPUT);
  pinMode(randPin, INPUT);
  pinMode(onIndicatorPin, OUTPUT);
  pinMode(swingIndicatorPin, OUTPUT);
  
  // Check swing and randomness sensors
  swing = digitalRead(swingPin);
  setTempoWithSwing();
  
  float tempRand = analogRead(randPin) / 1023.0;
  //tempRand = ((float) tempRand) / 1023.0;
  if (abs(tempRand - randomness) > 0.02) {
    randomness = tempRand;
    for (int i = 0; i < numDrums; i++) {
      drums[i]->setRandomness(randomness);
    }
  }

  // Define/Initialize drums
  for (int i = 0; i < numDrums; i++) {
    int pin = 22 + i;
    pinMode(pin, OUTPUT);
    drums[i] = new Drum(i, pin, swing, randomness);
    
    // temp
    drums[i]->setRandomness(0.1);
    d[i] = 0;
  }

  timer = 0;
  barLength = swing ? 8 : 16;
  isAlt = true;
  drumsOn = true;
  digitalWrite(onIndicatorPin, HIGH);

  samples = new Samples();
} //end setup

/****************
 * 
 *  LOOP
 *
 ****************/
void loop() {
  checkSensors();

  doDrums();
}// end loop

void doDrums() {
  int timeCheck = longBeat / 4.0;
  if (swing) {
    timeCheck = (isLongBeat ? longBeat : shortBeat);
  }
  if ((millis() - timer) > timeCheck) {
    timer = millis();
    isLongBeat = !isLongBeat;

    if (drumsOn/* && !debug*/) {
      for (int i = 0; i < numDrums; i++) {
        if (drums[i]->getDrumOn()) {
          d[i] = drums[i]->getHit(location[0], isAlt);
        }
        else {
          d[i] = 0;
        }
      }
      sprintf(output, "%d %d %d %d %d", d[0], d[1], d[2], d[3], d[4]);
      Serial.println(output);
    }

    // location[0] is position in current bar
    location[0] = (location[0] + 1) % barLength;
    if (location[0] == 0) {
      location[1] = (location[1] + 1) % 4;
      if (location[1] == 3) {
        isAlt = true;
        for (int i = 0; i < numDrums; i++) {
          drums[i]->newPattern(true);
        }
      }
      else { isAlt = false; }

      // location[1] is number of elapsed bars
      if (location[1] == 0) {
        // Pause at the end of a measure instead
        if (prepareToChange) { 
          drumsOn = !drumsOn; 
          prepareToChange = false;
          digitalWrite(onIndicatorPin, drumsOn ? LOW : HIGH);
        }
        
        int r;
        for (int i = 0; i < numDrums; i++) {
          // Probability of a new pattern increases by 20% each time
          changeProb[i] = changeProb[i] + 2;
          r = random(100);
          if (r < changeProb[i]) {
            drums[i]->newPattern(false);
            changeProb[i] = 0; // Reset to 0
          }
        }
      }
    }
  }
  if ((millis() - timer) > (timeCheck  / 2.0)) {
    for (int i = 0; i < numDrums; i++) {
      if (drums[i]->getDrumOn()) {
        drums[i]->turnOnLED();
      }
    }
  }
}

void calcTempo() {
  if (Utility::debug) {
    Serial.print("knock ");
    Serial.println(tempoCount);
  }
  
  if (tempoCount == 0) {
    lastKnock = millis();
    tempoCount = 1;
    diff = 0;
  }
  else {
    long tempMillis = millis();
    tempoCount++;
    diff = diff + (tempMillis - lastKnock);
    lastKnock = tempMillis;
  }

  if (tempoCount == 4) {
    // We're done; set the tempo
    totalTempo = ceil(diff / 4.0);
    lastKnock = 0;
    tempoCount = 0;

    // Calculate the tempo based on the tapped tempo
    setTempoWithSwing();
  }
}

void setTempoWithSwing() {
  if(swing) {
    swingTotalTempo = totalTempo;
    longBeat = swingTotalTempo * 0.67;
    shortBeat = swingTotalTempo * 0.33;
  }
  else {
    longBeat = totalTempo;
    shortBeat = totalTempo;
  }

  if (Utility::debug) {
    Serial.println(totalTempo);
    Serial.println(longBeat);
    Serial.println(shortBeat);
  }
  
  barLength = swing ? 8 : 16;
}

void clearTempo() {
  if (Utility::debug) {
    Serial.println("clearing");
  }
  lastKnock = 0;
  tempoCount = 0;
}

void pause(bool immediate) {
  if (Utility::debug) {
    Serial.println("Pausing");
  }
  if (immediate) {
    drumsOn = false;
  }
  else {
    prepareToChange = 1;
    digitalWrite(stopIndicatorPin, HIGH);
  }
}

void start() {
  if (Utility::debug) {
    Serial.println("Starting");
  }
  drumsOn = true;
  location[0] = 0;
  location[1] = 0;
  location[2] = 0;
}

void checkSensors() {
  digitalWrite(onIndicatorPin, drumsOn);

  // Check swing and randomness sensors
  int tempSwing = digitalRead(swingPin);
  if (tempSwing != swing) {
    swing = tempSwing;
    setTempoWithSwing();
  }

  float tempRand = analogRead(randPin) / 1023.0;
  //tempRand = ((float) tempRand) / 1023.0;
  if (abs(tempRand - randomness) > 0.02) {
    randomness = tempRand;
    for (int i = 0; i < numDrums; i++) {
      drums[i]->setRandomness(randomness);
    }
  }

  // Check MPR121 sensors
  for (int i = 0; i < numCaps; i++) {
    // Check if any of the electrode readings have changed
    int electrodeChanged = caps[i]->readTouchData();

    if (electrodeChanged > -1) {
      if (Utility::debug) {
        Serial.print("Electrode changed: ");
        Serial.println(electrodeChanged);
        Serial.println();
      }
      int drumIndex = ((i * numElectrodes) + electrodeChanged)/3;
      if (drumIndex < 5) {
        int newDensity = electrodeChanged % 3;
        drums[drumIndex]->setDensity(newDensity, caps[i]->getElectrodeOn(electrodeChanged, drumIndex % 4));
      }
      else {
        // Controls start/stop, tap tempo, samples for backing tracks
        switch (electrodeChanged) {
          case 3:
            start();
            break;
          case 4:
            pause(true);
            break;
          case 5:
            pause(false);
            break;
          case 6:
            if (tempoCount < 4) {
              calcTempo();
            }
            else if (lastKnock != 0 && (millis() - lastKnock) > maxWait) {
              clearTempo();
            }
            break;
          case 7:
            samples->sendSample();
            break;
          case 8:
            samples->goBack();
            break;
          case 9:
            samples->sampleReset();
            break;
        }
      }
    }
  }
} // end checkSensors
