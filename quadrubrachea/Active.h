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
  
  // To store the values when the toggle is initially set (for calculating the differential)
  int init_x, init_y, init_z;

  long init_clock;
  long clk;

public:
  Active(int which_arm);

  int* moveActive(int x, int y, int z, bool new_on);

};

#endif
