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

int potpin = 0;
int val;

void setup() {
  Serial.begin(115200);
  horiz.attach(9);
  verti.attach(8);

  horiz_ms = verti_ms = millis();
}

void loop() {
  calcHorizGoal();
  calcVertiGoal();
  moveServos();
}

void calcHorizGoal() {
  if (horiz_goal == horiz_curr) {
    
  }
}

void calcVertiGoal() {
  if (verti_goal == verti_curr) {
    
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
  }
  if (ms - verti_ms > verti_delay) {
    if (verti_goal < verti_curr) {
      verti_curr--;
    }
    else if (verti_goal > verti_curr) {
      verti_curr++;
    }
    verti.write(verti_curr);
  }
}
