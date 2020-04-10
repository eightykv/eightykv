#ifndef Utility_h
#define Utility_h

#include "Arduino.h"

const bool DEBUG = true;

// 4 arms
const int NUM_ARMS = 4;
// 4 joints per arm
const int NUM_JOINTS = 4;
// Range of motion on each joint
const int JOINT_ROM[NUM_JOINTS] = {180, 180, 180, 180};
// Start pins on the Mega board
const int START_PINS[NUM_ARMS] = {22/*, 28, 34, 40*/};

// State variables
const int SLEEP = 0;
const int TEST = 1;
const int INACTIVE = 2;
const int ACTIVE = 3;
const int FREEZE = 4;

struct activeData {
  int x;
  int y;
  int z;
  bool arm_on[4];
  bool arm_on_new[4];
};

static activeData global_active_data;

static void dLog(char *msg) {
  if (DEBUG) {
    Serial.println(msg);
  }
}

#endif
