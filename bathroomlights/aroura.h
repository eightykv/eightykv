#ifndef Aroura_h
#define Aroura_h

#include "Arduino.h"

#define MIN_WIDTH       20
#define MAX_WIDTH       50
#define MIN_CHANGE      2000
#define MAX_CHANGE      6000
#define ALT_MIN_CHANGE  -1000
#define ALT_MAX_CHANGE  600
#define MIN_SPEED       70
#define MAX_SPEED       30
#define NUM_LEDS        150

class Aroura {
private:
  int ndx;
  int width;
  int change;
  int change_per;
  int start_location;
  int update_ms;
  long last_update;
  bool dir;
  bool visible;
public:
  Aroura(int _ndx);
  void initialize(int base_color);
  void update_aroura();
  void finish();
  bool is_on();
  int get_start();
  int get_width();
  long get_change(int pixel);

  bool flag = false;
};

#endif
