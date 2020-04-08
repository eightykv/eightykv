#ifndef Active_h
#define Active_h

#include "Utility.h"

static int x;
static int y;
static int z;

static bool arm_on[4] = {false, false, false, false};
static bool arm_on_new[4] = {false, false, false, false};

/*
 * The Active class handles receiving data from the external inputs and 
 * translating it into motion
 */
class Active {
private:
  // Which arm did this correspond to?
  int which_arm;
  
  // To store the values when the toggle is initially set (for calculating the differential)
  int init_x;
  int init_y;
  int init_z;

  long init_clock;
  long clk;

public:
  Active(int which_arm);

  void moveActive();

};

#endif
