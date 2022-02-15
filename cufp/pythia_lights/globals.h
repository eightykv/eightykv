#ifndef globals_h
#define globals_h

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN     2
#define NUM_LEDS    30
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

extern int led_colors[NUM_LEDS][3];

extern Adafruit_NeoPixel leds;

#endif
