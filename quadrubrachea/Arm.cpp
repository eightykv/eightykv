#include "Arm.h"

Arm::Arm(int which_arm) {
  // Store which arm this is
  int start_pin = START_PINS[which_arm];
  this->which_arm = which_arm;
  
  // Attach each servo to its respective pin
  // Joint 0 is base, joint 3 is tip
  for (int i = 0; i < NUM_JOINTS; i++) {
    joints[i].attach(start_pin + i);
  }

  // Each arm has an Inactive object that manages idle state movement
  inactive = new Inactive();

  // Each arm has an Active object that manages active state movement
  active = new Active(which_arm);
}

void Arm::execute(int state, bool state_changed, activeData active_data) {
  if (state_changed) {
    clk = millis();
  }
  
  switch (state) {
    case SLEEP:
      sleepState(state_changed);
      break;
    case TEST:
      testState(state_changed);
      break;
    case INACTIVE:
      inactiveState(state_changed);
      break;
    case ACTIVE:
      activeState(state_changed, active_data);
      break;
    case FREEZE:
      break;
    default:
      dLog("Unexpected state");
      break;
  }
  
  moveArm();
}

void Arm::sleepState(bool state_changed) {
  destination_pos[0] = 90;
  destination_pos[1] = 120;
  destination_pos[2] = 179;
  destination_pos[3] = 120;
 
  for (int i = 0; i < NUM_JOINTS; i++) {
    //destination_pos[i] = 90;
    move_delay[i] = 20; // TODO: Slow down delay as it "goes to sleep"?
  }
}

/*
 * Tests joint ROM
 */
void Arm::testState(bool state_changed) {
  if (state_changed) {
    for (int i = 0; i < NUM_JOINTS; i++) {
      destination_pos[i] = arm_positions[position_index]->positions[which_arm][i];
      move_delay[i] = 30;
    }
  }

/*
  // If all joints have reached their destination...
  bool advance = true;
  for (int i = 0; i < NUM_JOINTS; i++) {
    if (current_pos[i] != destination_pos[i]) {
      advance = false;
    }
  }
  // First go to the maximum position, then the minimum, then end in the middle
  if (advance) {
    for (int i = 0; i < NUM_JOINTS; i++) {
      destination_pos[i] = (destination_pos[i] >= 179) ? 180 : 90;
    }
  }*/
}

void Arm::inactiveState(bool state_changed) {
  // This checks if a new inactive position has been selected
  inactive->checkInactive(which_arm, current_pos, destination_pos);
  // Update the destination to match what's selected
  for (int i = 0; i < NUM_JOINTS; i++) {
    int joint_position = inactive->getJointPosition(which_arm, i);
    if (destination_pos[i] != joint_position) {
      destination_pos[i] = joint_position;
    }
    // Calculate the delay time -- motor moves faster if it has a
    // long way to move, slower if it's almost at its destination
    int diff = abs(current_pos[i] - destination_pos[i]);
    if (diff > 50) {
      move_delay[i] = 20; // smallest delay = fastest move
    }
    else {
      move_delay[i] = 20 + ((1 - (diff / 50.0)) * MAX_INACTIVE_DELAY);
    }
  }
}

void Arm::activeState(bool state_changed, activeData active_data) {
  if (state_changed) {
    // Small move delay for active state
    for (int i = 0; i < NUM_JOINTS; i++) {
      move_delay[i] = 20;
    }
  }

  if (active_data.arm_on[which_arm]) {
    // Generate the active move
    int* offsets = active->moveActive(
      active_data.xyz,
      active_data.arm_on_new[which_arm]
    );
  
    // Update destination_pos using that data
    for (int i = 0; i < NUM_JOINTS; i++) {
      int npos = current_pos[i] + offsets[i];

      // Invert X axis for left side arms
      if (i == 0 && (which_arm == 1 || which_arm == 2)) {
        npos = current_pos[i] - offsets[i];
      }
      
      npos = npos < 0 ? 0 : npos;
      npos = npos > JOINT_ROM[i] ? JOINT_ROM[i] : npos;
      destination_pos[i] = npos;
    }
  }
  // If this arm isn't on, freeze it in place
  else {
    for (int i = 0; i < NUM_JOINTS; i++) {
      destination_pos[i] = current_pos[i];
    }
  }
}

void Arm::moveArm() {
  bool moved = false;
  for (int i = 0; i < NUM_JOINTS; i++) {
    // Error handling -- don't move if the destination is outside of range of motion
    if (destination_pos[i] < 0 || destination_pos[i] > JOINT_ROM[i]) {
      dLog("INVALID MOVE REQUESTED");
      return;
    }
    
    // If we're moving...
    if (current_pos[i] != destination_pos[i] && (millis() - clk) > move_delay[i] ) {
      moved = true;
      
      // Increase or decrease depending on which direction we're moving
      current_pos[i] = current_pos[i] > destination_pos[i] ?
                       current_pos[i] - 1 : current_pos[i] + 1;
      joints[i].write(current_pos[i]);
    }
  }
  if (moved) {
    clk = millis();
  }
}
