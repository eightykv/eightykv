#include "Arm.h"
#include "Utility.h"
#include "Wireless.h"

Arm *arms[NUM_ARMS];

// Initialize in SLEEP state
int state = SLEEP;
bool state_changed = true;

// To read from Serial
const byte MAX_CHARS = 9;
char in_buffer[MAX_CHARS];

void setup() {
  // No particular reason for 57600
  Serial.begin(57600);

  // Pins to set the state
  pinMode(ACTIVE_PIN, INPUT_PULLUP);
  pinMode(INACTIVE_PIN, INPUT_PULLUP);

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
  int active_read = digitalRead(ACTIVE_PIN);
  int inactive_read = digitalRead(INACTIVE_PIN);

  // If the switch is newly on, change to ACTIVE
  if (active_read == LOW && last_active_read == HIGH) {
    dLog("ACTIVE FROM SWITCH");
    state = ACTIVE;
    state_changed = true;
  }
  // If the switch is newly off, go to sleep
  if (active_read == HIGH && last_active_read == LOW) {
    dLog("SLEEP FROM SWITCH");
    state = SLEEP;
    state_changed = true;
  }
  
  if (inactive_read == LOW && last_inactive_read == HIGH) {
    dLog("INACTIVE FROM SWITCH");
    state = INACTIVE;
    state_changed = true;
  }
  if (inactive_read == HIGH && last_inactive_read == LOW) {
    dLog("SLEEP FROM SWITCH");
    state = SLEEP;
    state_changed = true;
  }

  last_active_read = active_read;
  last_inactive_read = inactive_read;

  // If we're in the active state, read wireless data
  if (state == ACTIVE) {
    readWireless();
  }

  static int ndx = 0;
  static bool new_data = false;
  while (Serial.available() && !new_data) {
    
    char c = Serial.read();
    if (c != '\n' && ndx < MAX_CHARS) {
      in_buffer[ndx] = c;
      ndx++;
    }
    else {
      in_buffer[ndx] = '\0';
      new_data = true;
    }
  }
  
  if (new_data) {
    int arm_num = in_buffer[0] - 48;
    int joint_num = in_buffer[2] - 48;
    int new_dest = atoi(in_buffer + 4);
    Serial.println((String) "Arm num: " + arm_num + "; joint_num: " + joint_num + "; new destination: " + new_dest);
    arms[arm_num]->setDestination(joint_num, new_dest);
    new_data = false;
    ndx = 0;
  }

    /*
    // If we receive serial input from 0-3, use it to set the state
    if (c >= '0' && c <= '4') {
      state_changed = true;
      // I could replace this with one line of code
      // But I'm not gonna because this is more readable
      switch (c) {
        case '0':
          /* SLEEP
           * Arms curl in for easier transport
           *
          state = SLEEP;
          dLog("SLEEP");
          break;
        case '1':
          /* TEST
           * Move each joint individually
           *
          state = TEST;
          dLog("TEST");
          break;
        case '2':
          /* INACTIVE
           * Arms move in a pseudorandom pattern
           *
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
    }*/
}
