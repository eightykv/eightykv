#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    10
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
long last_update;

int led_colors[NUM_LEDS][3];
int change_speed[3] = {250, 15, 15};
int goal[3] = {0, 0, 0};
int goal_min[3] = {0, 16, 0};
int goal_max[3] = {64, 256, 256};
int goal_delay_min[3] = {30, 10, 20};
int goal_delay_max[3] = {100, 50, 60};
long color_ms[3];

int pulse_delay = 42;
int pulse_min = 12;
int pulse_max = 80;
int pulse_current = 16;
bool increasing = true;
long pulse_ms;

#define UPDATES_PER_SECOND 100

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  delay( 3000 );
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  for (int i = 0; i < NUM_LEDS; i++) {
    for (int j = 0; j < 3; j++) {
      led_colors[i][j] = 0;
      color_ms[j] = millis();
    }
  }
  last_update = pulse_ms = millis();
}


void loop()
{
  updateGoals();
  updateColors();
  updatePulse();
  fillLEDs();

  if (millis() - last_update > UPDATES_PER_SECOND) {
    FastLED.show();
  }
}

void updateGoals() {
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

void updateColors() {
  long ms = millis();
  for (int j = 0; j < 3; j++) {
    if (ms - color_ms[j] > change_speed[j]) {
      if (led_colors[0][j] < goal[j]) {
        led_colors[0][j]++;
      }
      else if (led_colors[0][j] > goal[j]) {
        led_colors[0][j]--;
      }
      color_ms[j] = ms;
    }
  }
}

void updatePulse() {
  if (millis() - pulse_ms > pulse_delay) {
    if (increasing) {
      pulse_current = min(pulse_current + 1, pulse_max);
    }
    else {
      pulse_current = max(pulse_current - 1, pulse_min);
    }
    if (pulse_current == pulse_min || pulse_current == pulse_max) {
      increasing = !increasing;
    }
    FastLED.setBrightness(  pulse_current );
    pulse_ms = millis();
  }
}

void fillLEDs () {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(led_colors[0][0], led_colors[0][1], led_colors[0][2]);
  }
}
