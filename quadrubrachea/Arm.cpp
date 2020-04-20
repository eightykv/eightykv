#include "Arm.h"

Arm::Arm(int which_arm, int start_pin) {
  // Store which arm this is
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
    for (int i = 0; i < NUM_JOINTS; i++) {
      clk[i] = millis();
    }
  }
  
  switch (state) {
    case OFF:
      break;
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

  if (state != OFF) {
    moveArm();
  }
}

void Arm::sleepState(bool state_changed) {
  
    move_delay[0] = 60;
    move_delay[1] = 100;
    move_delay[2] = 60;
    move_delay[3] = 40;
    
  if (state_changed) {
    for (int i = 0; i < NUM_JOINTS; i++) {
      destination_pos[i] = 90;
      //move_delay[i] = 20; // TODO: Slow down delay as it "goes to sleep"?
    }
  }
}

/*
 * Tests joint ROM
 */
void Arm::testState(bool state_changed) {
  if (state_changed) {
    for (int i = 0; i < NUM_JOINTS; i++) {
      destination_pos[i] = JOINT_ROM[i];
      move_delay[i] = 20;
    }
  }

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
      destination_pos[i] = destination_pos[i] == JOINT_ROM[i] ? 0 : round(JOINT_ROM[i] / 2);
    }
  }
}

void Arm::inactiveState(bool state_changed) {
  bool new_motion = false;

  // If this is new, setup clocks and pick a new motion
  if (state_changed) {
    interval_clk = millis();
    new_motion = true;
  }

  // If it's been longer than the set interval, pick a new motion
  if (millis() - interval_clk > interval) {
    // Do something new
    new_motion = true;
    interval = 5000 + random(-4000, 2000);
  }
/*
  // If it's time to take the next step, do it
  if (millis() - clk >= step_delay) {
    // If we're advancing to a new motion, reset the clock
    if (new_motion) {
      interval_clk = millis();
    }
    
    // Get the next step
    step next_step = inactive->take_step(new_motion);

    // Use the step to populate the position/delay arrays
    int j = next_step.joint;
    
    // Get the new position; constrain by min/max
    int new_pos = current_pos[j] + next_step.position_offset;
    new_pos = max(new_pos, 0);
    new_pos = min(new_pos, JOINT_ROM[j]);
    destination_pos[j] = new_pos;

    // Set the delays
    move_delay[j] = next_step.move_delay;
    step_delay = next_step.step_delay;
  }*/
}

void Arm::activeState(bool state_changed, activeData active_data) {
  if (state_changed) {
    // Small move delay for active state
    move_delay[0] = 60;
    move_delay[1] = 90;
    move_delay[2] = 60;
    move_delay[3] = 40;
    /*
    for (int i = 0; i < NUM_JOINTS; i++) {
      move_delay[i] = 20;
    }*/
  }

  if (active_data.arm_on[which_arm]) {
    // Generate the active move
    int* offsets = active->moveActive(
      active_data.xyz,
      active_data.arm_on_new[which_arm]
    );

    // Don't move joint 1 unless joint 2 is maxed out
    /*
    if (current_pos[2] > 0 && current_pos[2] < JOINT_ROM[2]) {
      offsets[1] = 0;
    }*/
  
    // Update destination_pos using that data
    for (int i = 0; i < NUM_JOINTS; i++) {
      int new_pos = current_pos[i] + *(offsets + i);
      int max_pos = i > 0 ? round(JOINT_ROM[i] / 2) : JOINT_ROM[i];
      new_pos = new_pos < 0 ? 0 : new_pos;
      new_pos = new_pos > max_pos ? max_pos : new_pos;
      destination_pos[i] = new_pos;
      
      Serial.print(new_pos);
      Serial.print(" ");
    }
    Serial.println();
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
    if (current_pos[i] != destination_pos[i] && (millis() - clk[i]) > move_delay[i] ) {
      clk[i] = millis();
      
      // Increase or decrease depending on which direction we're moving
      current_pos[i] = current_pos[i] > destination_pos[i] ?
                       current_pos[i] - 1 : current_pos[i] + 1;
      joints[i].write(current_pos[i]);
    }
  }
}

