#ifndef Active_h
#define Active_h

#include "Utility.h"

/*
 * The Active class handles receiving data from the external inputs and 
 * translating it into motion
 */
class Active {
private:
  // Which arm did this correspond to?
  int which_arm;
  
  // Stores the values when the toggle is initially set (for calculating the differential)
  int init_x, init_y, init_z;

  // Tracks how long we've been moving in a particular direction
  int pos_tics = 0;
  int neg_tics = 0;
  int num_tics = 4; // How many tics before we start moving joint 1?

  long clk;

  // Helper functions to calculate the offsets for each joint
  int calc0(int x);
  int calc1(int y);
  int calc2(int y);
  int calc3(int z);

public:
  Active(int which_arm);

  int* moveActive(int x, int y, int z, bool new_on);

};

#endif
