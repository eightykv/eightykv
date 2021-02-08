/* DAYES
*/

#include <Servo.h>

const int NUM_SERVOS = 3;

/* DEBUG variable.
   -1: no debug
    0: debug tung
    1: debug wobble
    2: debug tube
*/
const int DEBUG = -1;

Servo tung;
int tung_i = 0;
int tung_mode = 0;
int sweep_counter = 0;
int tung_min_delay = 10;
int tung_max_delay = 50;

Servo wobble;
int wob_i = 1;

Servo tube;
int tube_i = 2;

Servo *servos[NUM_SERVOS];

int pos[NUM_SERVOS] = {0, 0, 0};
int goal[NUM_SERVOS] = {0, 0, 0};
int del[NUM_SERVOS] = {30, 15, 30};
int increment[NUM_SERVOS] = {1, 1, 1};
long last_update[NUM_SERVOS];

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));

  tung.attach(3);
  wobble.attach(4);
  tube.attach(5);

  servos[0] = &tung;
  servos[1] = &wobble;
  servos[2] = &tube;

  for (int i = 0; i < NUM_SERVOS; i++) {
    last_update[i] = millis();
  }
}

void loop() {
  updateGoals();
  writePos();
}

void writePos() {
  long m = millis();
  for (int i = 0; i < NUM_SERVOS; i++) {
    // Check if it's time to update the position
    if (m - last_update[i] > del[i]) {
      // If the goal is larger than the position, increment upwards
      if (goal[i] > pos[i]) {
        pos[i] += increment[i];
        pos[i] = min(goal[i], pos[i]);
      }
      // If it's smaller, increment downwards
      else if (goal[i] < pos[i]) {
        pos[i] -= increment[i];
        pos[i] = max(goal[i], pos[i]);
      }
      last_update[i] = m;

      updateDelays();

      // Write the new position
      servos[i]->write(pos[i]);
    }
  }
}

void updateGoals() {
  updateTungGoal();
  updateWobbleGoal();
  updateTubeGoal();
}

void updateTungGoal() {
  // Tung: 0 to 90, usually sweeps slowly down and quickly up, occasionally flutters in place
  // If we've reached our goal, time to change it
  if (goal[tung_i] == pos[tung_i]) {
    sweep_counter++;
    // Occasionally switch modes
    if (sweep_counter > 10) {
      if (tung_mode == 0) {
        if (random(100) <= sweep_counter - 10) {
          dlog(0, "Switch to mode 1");

          tung_mode = 1;
          sweep_counter = 0;
        }
      }
      else if (tung_mode == 1) {
        if (random(10) <= sweep_counter - 20) {
          dlog(0, "Switch to mode 0");

          tung_mode = 0;
          sweep_counter = 0;
        }
      }
    }

    int rand_offset = random(20);

    // 0 = sweep mode. Slow down, fast up
    if (tung_mode == 0) {
      // If our last goal was high, our new goal will be low, with a fast delay
      if (goal[tung_i] >= 45) {
        goal[tung_i] = 30 + rand_offset;
        del[tung_i] = random(40, 60);
      }
      // If our last goal was low, our new goal is high, with a slow delay
      else {
        goal[tung_i] = 80 - rand_offset;
        del[tung_i] = random(80, 100);
      }

      dlog(0, (String) "Mode 0. New goal: " + goal[tung_i] + "; new delay: " + del[tung_i]);
    }
    // 1 = flutter mode. Rapid, smaller motions
    if (tung_mode == 1) {
      if (goal[tung_i] >= 50) {
        goal[tung_i] = 55 - rand_offset;
      }
      else {
        goal[tung_i] = 55 + rand_offset;
      }
      del[tung_i] = random(40, 60);

      dlog(0, (String) "Mode 1. New goal: " + goal[tung_i] + "; new delay: " + del[tung_i]);
    }
  }
}

void updateWobbleGoal() {
  // Wobble: 0 to 180, moving pretty constantly, usually pretty slow but occasional quick sweep
  if (goal[wob_i] == pos[wob_i]) {
    // If it was high before, go low. Else go high
    if (goal[wob_i] >= 90) {
      goal[wob_i] = random(90);
    }
    else {
      goal[wob_i] = random(90, 180);
    }

    // Usually pretty slow
    del[wob_i] = random(15, 40);
    // 1 in 20 chance of go fast
    if (random(20) == 0) {
      del[wob_i] = random(5, 10);
    }

    dlog(1, (String) "New wobble goal: " + goal[wob_i] + "; new delay: " + del[wob_i]);
  }
}

void updateTubeGoal() {
  // Tube: 0 to 180, goes up and down quickly and settles into the middle
  if (goal[tube_i] == pos[tube_i]) {
    if (goal[tube_i] == 0) {
      goal[tube_i] = 179;
    }
    else {
      goal[tube_i] = 0;
    }
  }
}

static void dlog(int src, String msg) {
  if (DEBUG == src) {
    Serial.println(msg);
  }
}

void updateDelays() {
  updateTungDelay();
}

void updateTungDelay() {
  int last_del = del[tung_i];
  // Update delay for tung -- slow down as goal is reached
  if (abs(goal[tung_i] - pos[tung_i]) < 15) {
    del[tung_i] = min(120, del[tung_i] + 5);
  }
  else if (abs(goal[tung_i] - pos[tung_i]) > 25) {
    del[tung_i] = max(15, del[tung_i] - 5);
  }
  if (del[tung_i] != last_del) {
    //dlog(0, (String) "Tung delay: " + del[tung_i]);
  }
}
