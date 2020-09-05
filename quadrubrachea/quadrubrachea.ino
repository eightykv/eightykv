#include "Arm.h"
#include "Utility.h"
#include "Wireless.h"

Arm *arms[NUM_ARMS];

// Initialize in SLEEP state
int state = SLEEP;
bool state_changed = true;

void setup() {
  // No particular reason for 57600
  Serial.begin(57600);

  // Setup the wireless communication
  setupWireless();

  // Random seed
  randomSeed(analogRead(0));

  // Initialize the arms, which will initialize the servos
  for (int i = 0; i < NUM_ARMS; i++) {
    arms[i] = new Arm(i);
  }
}

void loop() {
  // Read possible state change
  readInstructions();

  // For each arm, execute the instructions
  for (int i = 0; i < NUM_ARMS; i++) {
    arms[i]->execute(state, state_changed, global_active_data);
  }
  
  state_changed = false;
}

void readInstructions() {
  if (Serial.available()) {
    // If we receive serial input from 0-3, use it to set the state
    char c = Serial.read();
    if (c >= '0' && c <= '4') {
      state_changed = true;
      // I could replace this with one line of code
      // But I'm not gonna because this is more readable
      switch (c) {
        case '0':
          /* SLEEP
           * Arms curl in for easier transport
           */
          state = SLEEP;
          dLog("SLEEP");
          break;
        case '1':
          /* TEST
           * Move each joint individually
           */
          state = TEST;
          dLog("TEST");
          break;
        case '2':
          /* INACTIVE
           * Arms move in a pseudorandom pattern
           */
          state = INACTIVE;
          dLog("INACTIVE");
          break;
        case '3':
          // ACTIVE
          state = ACTIVE;
          dLog("ACTIVE");
          break;
        case '4':
          // FREEZE
          state = FREEZE;
          dLog("FREEZE");
          break;
        default:
          break;
      }
    }
  }

  // If we're in the active state, read wireless data
  if (state == ACTIVE) {
    readWireless();
  }
}

