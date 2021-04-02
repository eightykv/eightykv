#include "breathe.h"

Breathe::Breathe() {
  long ms = millis();
  for (int j = 0; j < 3; j++) {
    color_ms[j] = ms;
  }
  breathe_ms = ms;
}

int Breathe::turnOff(int brightness_current) {
  if (brightness_current > breathe_min && millis() - breathe_ms > breathe_delay) {
    brightness_current--;
    breathe_ms = millis();
  }
  return brightness_current;
}

int Breathe::updateBreathe(int brightness_current) {
  int brightness = updateBrightness(brightness_current);
  updateGoals();
  updateColors();
  return brightness;
}

int Breathe::updateBrightness(int brightness_current) {
  if (millis() - breathe_ms > breathe_delay) {
    if (increasing) {
      brightness_current = min(brightness_current + 1, breathe_max);
    }
    else {
      brightness_current = max(brightness_current - 1, breathe_min);
    }
    if (brightness_current == breathe_min || brightness_current == breathe_max) {
      increasing = !increasing;
    }
    breathe_ms = millis();
  }
  return brightness_current;
}

void Breathe::updateGoals() {
  int sum = 0;
  for (int j = 0; j < 3; j++) {
    sum += goal[j];
    if (led_colors[0][j] == goal[j]) {
      goal[j] = min(random(goal_min[j], goal_max[j]), max(255 - sum, 0));
      //Serial.println((String) j + ": " + goal[j]);
      change_speed[j] = random(goal_delay_min[j], goal_delay_max[j]);
    }
  }
}

void Breathe::updateColors() {
  long ms = millis();
  for (int i = 0; i < NUM_LEDS; i++) {
    for (int j = 0; j < 3; j++) {
      if (ms - color_ms[j] > change_speed[j]) {
        if (led_colors[i][j] < goal[j]) {
          led_colors[i][j]++;
        }
        else if (led_colors[i][j] > goal[j]) {
          led_colors[i][j]--;
        }
        // On the last one, save the time
        if (i == NUM_LEDS - 1) {
          color_ms[j] = ms;
        }
      }
    }
  }
}
