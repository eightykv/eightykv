#ifndef pulse_h
#define pulse_h

#include "globals.h"

struct Pulse {
  int offset; // Maximum color offset from baseline -- only in green
  int actual_offsets[32];
  int width; // How large this pulse is. Stored as half of the total width
  int pulse_start; // The first pixel (lowest index) to have its color changed by this pulse
  int pulse_end; // The first pixel (lowest index) to have its color changed by this pulse
  bool move_direction; // true if up, false if down
  int move_delay; // How long to wait before moving to the next pixel
  long last_move; // The last time this moved
  bool active; // If this pulse is inactive, it'll be overwritten
};

//const int LED_FORMAT = NEO_GRB + NEO_KHZ800;
const int DEFAULT_COLOR[3] = {0, 0, 255};
const int TIME_BETWEEN = 200;
// Width is between one third and four fifths of the length of the strip
const int MIN_WIDTH = 8;
const int MAX_WIDTH = 10;
// Max color offset
const int MIN_OFFSET = 12;
const int MAX_OFFSET = 255;
// Min and max delay
const int MIN_DELAY = 20;
const int MAX_DELAY = 80;
// Max number of pulses
const int MAX_PULSES = 5;


class PulseClass {
private:
  Pulse *pulses[MAX_PULSES];
  int num_pulses = 0;
  long last_pulse;
  Pulse *newPulse(int index);
  float randFloat();
  void updatePixels();

public:
  PulseClass();
  void updatePulses();
};

#endif
