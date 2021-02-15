#include <Servo.h>

Servo horiz;
int horiz_curr = 90;
int horiz_goal = 90;
int horiz_delay = 20;
long horiz_ms;

Servo verti;
int verti_curr = 90;
int verti_goal = 90;
int verti_delay = 20;
long verti_ms;

bool pause = false;
int  pause_duration = -1;
long pause_ms;

int button_pin = 2;
int last_button_val = false;
int button_ms;
int retrig = 500;
int mode = 1;

void setup() {
  Serial.begin(115200);

  pinMode(button_pin, INPUT_PULLUP);
  
  horiz.attach(9);
  verti.attach(8);

  horiz_ms = verti_ms = pause_ms = button_ms = millis();
}

void loop() {
  checkButton();
  calcPause();
  calcHorizGoal();
  calcVertiGoal();
  if (!pause) {
    moveServos();
  }
}

void checkButton() {
  int button_val = digitalRead(button_pin);
  if (button_val == 0 && (button_val != last_button_val) && (millis() - button_ms > retrig)) {
    mode = !mode;
    button_ms = millis();
  }
  last_button_val = button_val;
}

void calcPause() {
  if (mode == 0 && !pause && (horiz_goal == horiz_curr || verti_goal == verti_curr)) {
    int r = random(10);
    if (r < 3) {
      pause = true;
      pause_duration = random(250, 800);
      pause_ms = millis();
    }
  }
  else if (millis() - pause_ms > pause_duration) {
    pause = false;
  }
}

void calcHorizGoal() {
  if (mode == 0) {
    if (horiz_goal == horiz_curr) {
      int last_goal = horiz_goal;
      horiz_goal = random(60, 120);
      int diff = abs(last_goal - horiz_goal);
      horiz_delay = 40 + min(0, 60 - diff);
    }
  }
  else {
    horiz_goal = 90;
    horiz_delay = 30;
  }
}

void calcVertiGoal() {
  if (mode == 0) {
    if (verti_goal == verti_curr) {
      verti_goal = random(76, 98);
      verti_delay = 50 + random(70);
    }
  }
  else {
    verti_goal = 90;
    verti_delay = 90;
  }
}

void moveServos() {
  long ms = millis();
  if (ms - horiz_ms > horiz_delay) {
    if (horiz_goal < horiz_curr) {
      horiz_curr--;
    }
    else if (horiz_goal > horiz_curr) {
      horiz_curr++;
    }
    horiz.write(horiz_curr);
    horiz_ms = ms;
  }
  if (ms - verti_ms > verti_delay) {
    if (verti_goal < verti_curr) {
      verti_curr--;
    }
    else if (verti_goal > verti_curr) {
      verti_curr++;
    }
    verti.write(verti_curr);
    verti_ms = ms;
  }
}
