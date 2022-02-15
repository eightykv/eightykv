#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_MPR121.h>
#include "aroura.h"

#define LED_PIN    2
#define LED_COUNT 150

Adafruit_MPR121 cap = Adafruit_MPR121();
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
int b = 0;
int b_vals[4] = {0, 20, 90, 255};
long touch_start[4] = {-1, -1, -1, -1};
long base_color = 5000;
bool cycle_color = false;

const int NUM_AROURAS = 8;
Aroura *arouras[NUM_AROURAS];
bool do_aroura = false;
long last_aroura = -1;
int aroura_wait = 0;

bool daylight_mode = false;

long pixel_vals[LED_COUNT];

void setup() {
  Serial.begin(57600);

  randomSeed(analogRead(A0));
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
  
  strip.begin();
  strip.setBrightness(0);

  for (int i = 0; i < NUM_AROURAS; i++) {
    arouras[i] = new Aroura(i);
  }
  
  strip.show();
}


void loop() {
  for (int i = 0; i < LED_COUNT; i++) {
    pixel_vals[i] = base_color;
  }
  
  getTouch();
  updateAroura();
  updateLEDs();
}

void getTouch() {
  
  // Get the currently touched pads
  currtouched = cap.touched();

  for (int i = 0; i < 4; i++) {
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && touch_start[i] < 0 ) {
      touch_start[i] = millis();
      //Serial.println(i);
      switch (i) {
        case 0:
          b = (b + 1) % 4;
          strip.setBrightness(b_vals[b]);
          break;
        case 1:
          cycle_color = !cycle_color;
          Serial.println(base_color);
          break;
        case 2:
          do_aroura = !do_aroura;
          if (!do_aroura) {
              last_aroura = -1;
              for (int i = 0; i < NUM_AROURAS; i++) {
                arouras[i]->finish();
              }
          }
          break;
        case 3:
          daylight_mode = !daylight_mode;
          if (daylight_mode) {
            strip.setBrightness(255);
          }
          else {
            strip.setBrightness(b_vals[b]);
          }
          break;
        default:
          break;
      }
    }

    // Wait at least 250ms before this one can be triggered again
    if (!(currtouched & _BV(i)) && touch_start[i] > 250 ) {
      touch_start[i] = -1;
    }
  }

  // reset our state
  lasttouched = currtouched;
}

void updateAroura() {
  if (do_aroura) {
    for (int i = 0; i < NUM_AROURAS; i++) {
      if (last_aroura < 0 || millis() - last_aroura >= aroura_wait) {
        bool found = false;
        
        // Add a new pulse
        if (!found && !arouras[i]->is_on()) {
          //Serial.println((String) "Initializing aroura " + i);
          arouras[i]->initialize(base_color);
          aroura_wait = random(500, 1000);
          last_aroura = millis();
          found = true;
        }
      }
      
      arouras[i]->update_aroura();

      if (arouras[i]->is_on()) {
        int start_pixel = arouras[i]->get_start();
        //Serial.println(start_pixel);
        int width = arouras[i]->get_width();
        for (int j = 0; j < width * 2; j++) {
          int curr_pixel = start_pixel + j;
          if (curr_pixel >= 0 && curr_pixel < LED_COUNT) {
            long change = arouras[i]->get_change(j);
            pixel_vals[curr_pixel] = (pixel_vals[curr_pixel] + change) % 65535;
          }
        }
      }
    }
  }
}

void updateLEDs() {
  if (cycle_color) {
    base_color = (base_color + 16) % 65535;
  }
  
  for(int i = 0; i < LED_COUNT; i++) {
    long c = strip.gamma32(strip.ColorHSV(pixel_vals[i]));
    if (daylight_mode) {
      c = strip.gamma32(strip.ColorHSV(0, 0, 255));
    }
    strip.setPixelColor(i, c);
  }
  
  strip.show();
  delay(10);
}
