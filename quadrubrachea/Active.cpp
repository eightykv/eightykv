#include "Active.h"
#include "Arduino.h"

Active::Active() {

}

void Active::moveActive(int which_arm) {
  // If this arm is active...
  if (arm_on[which_arm]) {
    // If it's _newly_ on...
    if (arm_on_new[which_arm]) {
      // Store the X, Y, and Z positions from when this was initially activated
      init_x = x;
      init_y = y;
      init_z = z;

      // Store when this move began
      clk = init_clock = millis();
      
      arm_on_new[which_arm] = false;
    }
    
    // Calculate the movement

    // How long before I start moving the next joint?
    // Do I only use Y and Z? I think so
  }
}

