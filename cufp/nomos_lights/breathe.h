#ifndef breathe_h
#define breathe_h

#include "globals.h"

class Breathe {
private:
  int change_speed[3] = {250, 15, 15};
  int goal[3] = {0, 0, 0};
  int goal_min[3] = {0, 16, 0};
  int goal_max[3] = {64, 256, 256};
  int goal_delay_min[3] = {30, 10, 20};
  int goal_delay_max[3] = {100, 50, 60};
  long color_ms[3];
  
  int breathe_delay = 5;
  int breathe_min = 4;
  int breathe_max = 200;
  int brightness_current = 16;
  bool increasing = true;
  long breathe_ms;
  
  int updateBrightness(int brightness_current);
  void updateGoals();
  void updateColors();
  
public:
  Breathe();
  int updateBreathe(int brightness_current);
  int turnOff(int brightness_current);
};

#endif
