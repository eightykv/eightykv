#include "Active.h"
#include "Arduino.h"

Active::Active(int which_arm) {
  this->which_arm = which_arm;
}

int* Active::moveActive(int xyz[3], bool new_on) {
  static int offsets[4] = {0, 0, 0, 0};
  
  // If this arm is active...
  // If it's _newly_ on...
  if (new_on) {
    // Store the X, Y, and Z positions from when this was initially activated
    for (int i = 0; i < 3; i++) {
      init_xyz[i] = xyz[i];
      prev_xyz[i] = xyz[i];
    }

    // Store when this move began
    clk = millis();
  }
  
  // Calculate the movement - sample every 100ms
  if (millis() - clk > 100) {
    clk = millis();

    // Calculate the offsets
    offsets[0] = 0 - calcOffset(xyz[0], prev_xyz[0], init_xyz[0], 30);
    offsets[1] = calcOffset(xyz[1], prev_xyz[1], init_xyz[1], 60);
    offsets[2] = calcOffset(xyz[1], prev_xyz[1], init_xyz[1], 30);
    offsets[3] = calcOffset(xyz[2], prev_xyz[2], init_xyz[2], 30);

    for (int i = 0; i < 3; i++) {
      prev_xyz[i] = xyz[i];
    }
  }
  
  return offsets;
}

/*
 * Calculate the changes to position
 * If the total change exceeds a particular threshold, start moving in a direction
 * If movement in another direction happens, stop moving and reset the initial value
 */
int Active::calcOffset(int val, int prev_val, int &init_val, int toggle_val) {
  int total_change = val - init_val;
  int latest_change = val - prev_val;
  bool same_dir = ((total_change ^ latest_change) >= 0);
  
  // Edge case for X axis when it jumps from 0 to 360 or vise versa
  if (abs(total_change) > 300 ||abs(latest_change) > 300 ) {
    init_val = val;
    total_change = 0;
    latest_change = 0;
  }

  if (same_dir || latest_change == 0) {
    return (0 - floor(total_change / toggle_val));
  }
  
  // Stop a motion by going in the opposite direction
  init_val = val;
  return 0;
}

