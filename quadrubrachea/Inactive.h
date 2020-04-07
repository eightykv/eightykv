#ifndef Inactive_h
#define Inactive_h

#include "Utility.h"

// Information on a single step to be taken by a single joint
struct step {
  int joint;            // Which joint is moving?
  int position_offset;  // How far is it moving? (positive or negative offset from current position)
  int move_delay;       // How fast is it moving? (smaller = faster)
  int step_delay;       // How long until the next step?
};

/*
 * The Inactive class builds a "sequence" of semirandom moves based on a particular
 * template. This sequence is then used to populate the position and delay variables
 * within the Arm class, which moves the arm.
 */
class Inactive {
private:
  // Bank of possible motions
  static const int NUM_MOTIONS = 5;
  static const int PAUSE = 0;
  static const int TWITCH = 1;
  static const int WAVE = 2;
  static const int CURL = 3;
  static const int UNCURL = 4;

  // Motion functions
  step makeStep(int joint_probs[3], int pos_offs[2], int move_del[2], int step_del[2]);
  step pause();
  step twitch();
  step wave();
  step curl();
  step uncurl();

  // The current motion
  long current_motion;

public:
  Inactive();

  step take_step(bool new_motion);

};

#endif
