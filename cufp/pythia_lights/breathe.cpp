#include "breathe.h"

Breathe::Breathe() {
  long ms = millis();
  for (int j = 0; j < 3; j++) {
    color_ms[j] = ms;
  }
  breathe_ms = ms;
}

void Breathe::updateBreathe() {
  updateBrightness();
  updateGoals();
  updateColors();
}

void Breathe::updateBrightness() {
  if (millis() - breathe_ms > breathe_delay) {
    if (increasing) {
      breathe_current = min(breathe_current + 1, breathe_max);
    }
    else {
      breathe_current = max(breathe_current - 1, breathe_min);
    }
    if (breathe_current == breathe_min || breathe_current == breathe_max) {
      increasing = !increasing;
    }
    leds.setBrightness(  breathe_current );
    breathe_ms = millis();
  }
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
