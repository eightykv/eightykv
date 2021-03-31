#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"

#define LED_PIN    2
#define LED_COUNT 150

Adafruit_MPR121 cap = Adafruit_MPR121();
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
bool light_on = false;
int state = 0;

const int values[5][3] = 
{
  {255, 4, 0},
  {100, 100, 180},
  {230, 32, 0},
  {180, 200, 16},
  {255, 16, 50}
};

void setup() {
  Serial.begin(115200);
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
  
  strip.begin();
  strip.show();
  strip.setBrightness(64);
}


void loop() {
  getTouch();
  updateLEDs();
}

void getTouch() {
  
  // Get the currently touched pads
  currtouched = cap.touched();
  
  if ((currtouched & _BV(0)) && !(lasttouched & _BV(0))) {
    Serial.println("Touched");
    light_on = !light_on;
  }
  
  // reset our state
  lasttouched = currtouched;
}

void updateLEDs() {

  int num_colors = 5;
  for (int i = 0; i < LED_COUNT; i++) {
    int r = values[i % num_colors][0];
    int g = values[i % num_colors][1];
    int b = values[i % num_colors][2];
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
  delay(10);

  /*
  if (cycle_color) {
    base_color = (base_color + 40) % 65536;
  }

  long c = strip.Color(0, 0, 0);
  if (light_on) {
    c = strip.gamma32(strip.ColorHSV(base_color));
  }

  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  
  strip.show();
  delay(10);*/
}
