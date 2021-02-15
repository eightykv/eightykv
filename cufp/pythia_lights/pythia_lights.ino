#include "globals.h"
#include "breathe.h"
#include "pulse.h"

long last_update;
Breathe breathe;
PulseClass pulse;

// Button info to change modes
int button_pin = 3;
int last_button_val = false;
int button_ms;
int retrig = 500;
int mode = 1;

int led_colors[NUM_LEDS][3];
Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

#define UPDATES_PER_SECOND 100

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  pinMode(button_pin, INPUT_PULLUP);
  delay( 3000 );

  breathe = *(new Breathe());
  pulse = *(new PulseClass());

  leds.begin();

  long ms = millis();
  last_update = button_ms = ms;
  
  for (int i = 0; i < NUM_LEDS; i++) {
    for (int j = 0; j < 3; j++) {
      led_colors[i][j] = 0;
    }
  }
}


void loop()
{
  checkButton();

  if (mode == 1) {
    breathe.updateBreathe();
  }
  else {
    pulse.updatePulses();
  }
  fillLEDs();

  if (millis() - last_update > UPDATES_PER_SECOND) {
    leds.show();
    last_update = millis();
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

void fillLEDs () {
  //Serial.println((String) led_colors[0][0] + ", " + led_colors[0][1] + ", " + led_colors[0][2]);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, leds.Color(led_colors[i][0], led_colors[i][1], led_colors[i][2]));
  }
}
