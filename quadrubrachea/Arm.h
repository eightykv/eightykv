#ifndef Arm_h
#define Arm_h

#include "Utility.h"
#include "Inactive.h"
#include "Active.h"
#include <Servo.h>

// We actually only need one "Inactive" object
static Inactive *inactive;

class Arm {
private:
  // Which arm is this (0-3)
  int which_arm;
  
  // Sleep state variables

  // Test state variables
  int position_index = 4;
  
  // Inactive state variables - per arm
  long clk = -1;
  long interval_clk = -1;
  int interval = 5000;
  int step_delay = 0;

  // Active state variables
  Active *active;

  // Movement variables
  int current_pos[4] = {89, 119, 178, 119};
  int destination_pos[4] = {90, 120, 179, 120}; 
  int move_delay[4] = {20, 20, 20, 20};
  
  // Array of servo "joints"
  Servo joints[NUM_JOINTS];
  // Move arm based on position/speed arrays
  void moveArm();

  // Helper functions handle the complexity of each movement
  void sleepState(bool state_changed);
  void testState(bool state_changed);
  void inactiveState(bool state_changed);
  void activeState(bool state_changed, activeData active_data);

public:
  // Constructor
  Arm(int which_arm);

  // Executes the current state -- public function exposed for each arm
  void execute(int state, bool state_changed, activeData active_data);
  
};

#endif
