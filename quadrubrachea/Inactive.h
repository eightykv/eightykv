#ifndef Inactive_h
#define Inactive_h

#include "Utility.h"

// How many different positions can we cycle through?
const int num_positions = 4;

// Information on a single step to be taken by a single joint
struct arm_position {
  int positions[4][4];
};
// List of inactive positions
const arm_position ap0 = {{
  {90, 120, 179, 120}, // Arm 0 joints 0-3
  {90, 120, 179, 120}, // Arm 1 joints 0-3
  {90, 120, 179, 120}, // Arm 2 joints 0-3
  {90, 120, 179, 120}  // Arm 3 joints 0-3
}};
const arm_position ap1 = {{
  {90, 70, 120, 60},
  {90, 70, 120, 60},
  {90, 70, 120, 60},
  {90, 70, 120, 60}
}};
const arm_position ap2 = {{
  {40, 120, 20, 20},
  {60, 90, 90, 90},
  {60, 90, 90, 90},
  {60, 90, 90, 90}
}};
const arm_position ap3 = {{
  {100, 140, 110, 20},
  {60, 90, 90, 90},
  {60, 90, 90, 90},
  {60, 90, 90, 90}
}};
const static arm_position *arm_positions[num_positions] = {&ap0, &ap1, &ap2, &ap3};
const int MAX_INACTIVE_DELAY = 80;

// Static because these need to be available across all arms
static int inactive_pos = 0;
static long inactive_changed;
static bool do_new_inactive_pos[4] = {false, false, false, false};
static int inactive_wait_min = 500;
static int inactive_wait_max = 2000;
static int inactive_wait = -1; // How long we hold this position before moving on

/*
 * The Inactive class builds a "sequence" of semirandom moves based on a particular
 * template. This sequence is then used to populate the position and delay variables
 * within the Arm class, which moves the arm.
 */
class Inactive {
private:
public:
  Inactive();
  void checkInactive(int which_arm, int current_pos[4], int destination_pos[4]);
  int getJointPosition(int which_arm, int which_joint);
};

#endif
