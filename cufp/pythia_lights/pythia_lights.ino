#include "globals.h"
#include "breathe.h"

long last_update;
Breathe breathe;

int led_colors[NUM_LEDS][3];
Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

#define UPDATES_PER_SECOND 100

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  delay( 3000 );

  breathe = *(new Breathe());

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
    last_update = millis();
  }
}

void fillLEDs () {
  //Serial.println((String) led_colors[0][0] + ", " + led_colors[0][1] + ", " + led_colors[0][2]);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, leds.Color(led_colors[i][0], led_colors[i][1], led_colors[i][2]));
  }
}
