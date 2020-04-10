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
    clk = millis();
  }
  
  // Calculate the movement - sample every 100ms
  if (millis() - clk > 100) {
    clk = millis();

    offsets[0] = calc0(x);
    offsets[1] = calc1(y);
    offsets[2] = calc2(y);
    offsets[3] = calc3(z);
  }
  
  return offsets;
}

/* 
 * Joint 0: pretty direct translation for X axis
 */
int Active::calc0(int x) {
  int diff = x - init_x;
  if (abs(diff) > 3) {
    // If we went from 0 to 360 or vise versa, we don't _actually_ want to move that far
    // X is the only axis that behaves this way; others go from 0 to +\- 180
    if (abs(diff) > 300) {
      diff = 0;
    }
    
    init_x = x;

    return ceil(diff * 0.5);
  }
  return 0;
}

/* 
 * Y axis controls joints 1 and 2.
 * When it first moves, start with joint 2. If it continues moving in that same
 * direction for multiple "tics" (100ms blocks), begin moving joint 1.
 * This means it'll start with the smaller motion (closer to the tip of the arm)
 * and only propegate down towards the shoulder as the motion continues.
 */

int Active::calc1(int y) {
  int diff = y - init_y;
  if (abs(diff) > 3) {
    init_y = y;
    
    // If motion has continued in one direction for long enough...
    if (pos_tics > num_tics || neg_tics > num_tics) {
      return ceil(diff * 0.5);
    }
  }
  return 0;
}

int Active::calc2(int y) {
  int diff = y - init_y;
  if (abs(diff) > 3) {
    if (diff > 0) {
      pos_tics++;
      neg_tics = 0;
    }
    else {
      neg_tics++;
      pos_tics = 0;
    }

    return ceil(diff * 0.5);
  }
}

/*
 * Z axis controls joint 3. Similar to X in being more or less direct mapping
 */
int Active::calc3(int z) {
  int diff = z - init_z;
  if (abs(diff) > 2) {
    init_z = z;
    return diff;
  }
}

