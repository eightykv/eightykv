#include "Inactive.h"
#include "Arduino.h"

Inactive::Inactive() {
  inactive_pos = random(num_positions);
  inactive_changed = millis();
}

void Inactive::checkInactive(int which_arm, int current_pos[4], int destination_pos[4]) {
  // Only do this processing if this arm isn't ready
  if (!do_new_inactive_pos[which_arm]) {
    // Determine whether we need to move to a new inactive position.
    // If all joints are in position, this arm is ready
    bool new_pos = true;
    for (int i = 0; i < NUM_JOINTS; i++) {
      if (current_pos[i] != destination_pos[i]) {
        new_pos = false;
      }
    }
    // Debug printout
    /*
    if (new_pos && !do_new_inactive_pos[which_arm]) {
      Serial.print("Arm ");
      Serial.print(which_arm);
      Serial.println(" ready");
    }*/
    do_new_inactive_pos[which_arm] |= new_pos; // Keep a record of which arms are ready
  }

  // If we're not already waiting...
  if (inactive_wait < 0) {
    // If all arms are in position, it's time to pick a new position
    for (int i = 0; i < NUM_ARMS; i++) {
      // If not all arms are in position yet, move on
      if (!do_new_inactive_pos[which_arm]) {
        return;
      }
    }
    // Determine how long we're gonna wait
    inactive_wait = random(inactive_wait_min, inactive_wait_max);
    // Store the current time
    inactive_changed = millis();
    /*
    Serial.print("Moving to new inactive position. Waiting ");
    Serial.println(inactive_wait);*/
  }
  else if (millis() - inactive_changed > inactive_wait) {
    // If the wait time has elapsed, it's time to pick a new position
    inactive_pos = random(num_positions);
    /*
    Serial.print("New inactive position: ");
    Serial.println(inactive_pos); */
    // Reset the variables
    inactive_wait = -1;
    inactive_changed = millis();
    for (int i = 0; i < NUM_ARMS; i++) {
      do_new_inactive_pos[i] = false;
    }
  }
}

int Inactive::getJointPosition(int which_arm, int which_joint) {
  return arm_positions[inactive_pos]->positions[which_arm][which_joint];
}
