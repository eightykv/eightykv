#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"

#define LED_PIN    2
#define LED_COUNT 150

Adafruit_MPR121 cap = Adafruit_MPR121();
uint16_t lasttouched = 0;
uint16_t currtouched = 0;
int retrig_ms = 200;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
bool cycle_color = false;
bool stop_cycle = false;
long touch_start = -1;
bool touch = false;
long base_color = 4800;

long brightness_clock;
int brightness = 0;
int brightness_states[5] = {0, 20, 64, 128, 255};
int state = 0;

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
  
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(base_color)));
  }
  
  strip.show();
  strip.setBrightness(brightness);
  touch_start = brightness_clock = millis();
}


void loop() {
  getTouch();

  updateLEDs();
}

void getTouch() {
  int diff = cap.baselineData(0) - cap.filteredData(0);
  
  if (diff >= 100 && touch == false && millis() - touch_start > retrig_ms) {
    touch = true;
    Serial.println("Touched");
    state = (state + 1) % 5;
    
    brightness = brightness_states[state];
    Serial.println((String) "State: " + state + "; Brightness: " + brightness);
    
    
    touch_start = millis();
  }
    
  if (diff <= 20 ) {
    //Serial.println("Released");
    touch_start = -1;
    touch = false;
  }

  // reset our state
  lasttouched = currtouched;
}

void updateLEDs() {
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(base_color)));
  }
  strip.setBrightness(brightness);
  strip.show();
}
