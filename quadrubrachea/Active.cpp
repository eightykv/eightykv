#include "Active.h"
#include "Arduino.h"

Active::Active(int which_arm) {
  this->which_arm = which_arm;
}

int* Active::moveActive(int x, int y, int z, bool new_on) {
  static int offsets[4] = {0, 0, 0, 0};
  
  // If this arm is active...
  // If it's _newly_ on...
  if (new_on) {
    // Store the X, Y, and Z positions from when this was initially activated
    init_x = x;
    init_y = y;
    init_z = z;

    // Store when this move began
    clk = init_clock = millis();
  }
  
  // Calculate the movement
  if (millis() - clk > 100) {
    clk = millis();
    if (abs(x - init_x) > 5) {
      offsets[0] = x - init_x; // Positive if increasing, negative if decreasing
      init_x = x;
    }

    init_y = y;
    init_z = z;
  }
  
  return offsets;
}

