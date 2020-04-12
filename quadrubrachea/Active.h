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
  int init_xyz[3];
  // Stores the previous XYZ value
  int prev_xyz[3];

  // Clock
  long clk;

  int calcOffset(int val, int prev_val, int &init_val, int toggle_val);

public:
  Active(int which_arm);

  int* moveActive(int xyz[3], bool new_on);

};

#endif
