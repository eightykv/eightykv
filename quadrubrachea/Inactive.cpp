#include "Inactive.h"
#include "Arduino.h"

Inactive::Inactive() {
  
}

step Inactive::take_step(bool new_motion) {
  if (new_motion) {
    // If we're supposed to pick a new motion, do it here
    int r = random(NUM_MOTIONS);
    // Only 33% chance of picking the same motion
    while (current_motion == r && random(100) > 33) {
      r = random(NUM_MOTIONS);
    }
    current_motion = r;
  }
  switch (current_motion) {
    case PAUSE:
      if (new_motion) { dLog("PAUSE"); }
      return pause();
    case TWITCH:
      if (new_motion) { dLog("TWITCH"); }
      return twitch();
    case WAVE:
      if (new_motion) { dLog("WAVE"); }
      return wave();
    case CURL:
      if (new_motion) { dLog("CURL"); }
      return curl();
    case UNCURL:
      if (new_motion) { dLog("UNCURL"); }
      return uncurl();
    default:
      break;
  }
}

step Inactive::makeStep(int joint_probs[3], int pos_offs[2], int move_del[2], int step_del[2]) {
  step next_step;
  
  int r = random(100);
  int j = -1;
  if (r >= joint_probs[2]) { j = 3; }
  else if (r < joint_probs[2] && r >= joint_probs[1]) { j = 2; }
  else if (r < joint_probs[1] && r >= joint_probs[0]) { j = 1; }
  else { j = 0; }
  next_step.joint = j;

  r = random(pos_offs[0], pos_offs[1]);
  next_step.position_offset = r;

  r = random(move_del[0], move_del[1]);
  next_step.move_delay = r;
  
  r = random(step_del[0], step_del[1]);
  next_step.step_delay = r;

  return next_step;
}

/*
 * PAUSE motion
 * Stop in place
 */
step Inactive::pause() {
  // Equal probability
  int joint_probs[3] = {25, 50, 75};
  // No position offset
  int pos_offs[2] = {0, 0};
  // Very long move delay
  int move_del[2] = {1000, 1000};
  // Very long step delay
  int step_del[2] = {1000, 1000};
  
  // Make the step and return
  return makeStep(joint_probs, pos_offs, move_del, step_del);
}

/*
 * TWITCH motion
 * Small, erratic movements, short delays
 */
step Inactive::twitch() {
  // Weighted towards the furthest out
  int joint_probs[3] = {10, 33, 63};
  // Small position offset
  int pos_offs[2] = {-3, 3};
  // Very short move delay
  int move_del[2] = {2, 5};
  // Short step delay
  int step_del[2] = {50, 200};
  
  // Make the step and return
  return makeStep(joint_probs, pos_offs, move_del, step_del);
}

/*
 * WAVE motion
 * Slowly move random joints
 */
step Inactive::wave() {
  // Equal probabilities
  int joint_probs[3] = {25, 50, 75};
  // Large position offset
  int pos_offs[2] = {-12, 12};
  // Long move delay
  int move_del[2] = {25, 55};
  // Short step delay
  int step_del[2] = {100, 200};
  
  // Make the step and return
  return makeStep(joint_probs, pos_offs, move_del, step_del);
}

/*
 * CURL motion
 * Slowly curls either inward or outward
 */
step Inactive::curl() {
  // Equal probabilities
  int joint_probs[3] = {5, 50, 75};
  // Medium position offset (always +)
  int pos_offs[2] = {0, 20};
  // Medium move delay
  int move_del[2] = {20, 25};
  // Short step delay
  int step_del[2] = {20, 50};
  
  // Make the step and return
  return makeStep(joint_probs, pos_offs, move_del, step_del);
}

/*
 * UNCURL motion
 * Slowly curls either inward or outward
 */
step Inactive::uncurl() {
  // Equal probabilities
  int joint_probs[3] = {5, 50, 75};
  // Medium position offset (always +)
  int pos_offs[2] = {-20, 0};
  // Slow move delay
  int move_del[2] = {20, 25};
  // Short step delay
  int step_del[2] = {20, 50};
  
  // Make the step and return
  return makeStep(joint_probs, pos_offs, move_del, step_del);
}

