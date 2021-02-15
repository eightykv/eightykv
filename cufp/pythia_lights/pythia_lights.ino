#include "globals.h"
#include "breathe.h"

long last_update;
Breathe breathe;

#define UPDATES_PER_SECOND 100

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  delay( 3000 );

  breathe = *(new Breathe());

  leds = new Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
  leds.begin();

  long ms = millis();
  last_update = ms;
  
  for (int i = 0; i < NUM_LEDS; i++) {
    for (int j = 0; j < 3; j++) {
      led_colors[i][j] = 0;
    }
  }
}


void loop()
{
  breathe.updateBreathe();
  fillLEDs();

  if (millis() - last_update > UPDATES_PER_SECOND) {
    leds.show();
  }
}

void fillLEDs () {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, led_colors[i][0], led_colors[i][1], led_colors[i][2]);
  }
}
