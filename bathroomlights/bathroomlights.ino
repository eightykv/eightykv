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
bool cycle_color = false;
bool stop_cycle = false;
long touch_start = -1;
long base_color = 5500;

void setup() {
  Serial.begin(57600);
  
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
  
  if ((currtouched & _BV(0)) && !(lasttouched & _BV(0)) && touch_start < 0 ) {
    Serial.println("Touched");
    touch_start = millis();
    if (cycle_color) {
      stop_cycle = true;
    }
    
    Serial.print("Color: ");
    Serial.println(base_color);
  }
  
  // If we held it for more than a second, cycle color
  if (light_on && !cycle_color && (currtouched & _BV(0)) && ((millis() - touch_start) > 1000 )) {
    cycle_color = true;
    Serial.println("Start cycle");
  }
    
  if (!(currtouched & _BV(0)) && (lasttouched & _BV(0)) && touch_start > 0 ) {
    Serial.println("Released");
    touch_start = -1;
    if (cycle_color) {
      light_on = true;
      if (stop_cycle) {
        cycle_color = false;
        stop_cycle = false;
        Serial.println("stop cycle");
      }
    }
    else {
      light_on = !light_on;
    }
    
    Serial.print("Light on: ");
    Serial.println(light_on);
  }

  // reset our state
  lasttouched = currtouched;
}

void updateLEDs() {
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
  delay(10);
}
