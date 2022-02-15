#include "globals.h"
#include "breathe.h"
#include "pulse.h"
#include <SoftwareSerial.h>
#include <SPI.h>
#include <RF24.h>

long last_update;
Breathe breathe;
PulseClass pulse;

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
char data_arr[4];

// Button info to change modes
int button_pin = 3;
int last_button_val = false;
int button_ms;
int retrig = 500;
int mode = 1;

int led_colors[NUM_LEDS][3];
int brightness_current = 4;
Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

#define UPDATES_PER_SECOND 100

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  pinMode(button_pin, INPUT_PULLUP);

  breathe = *(new Breathe());
  pulse = *(new PulseClass());

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  leds.begin();

  long ms = millis();
  last_update = button_ms = ms;
  
  for (int i = 0; i < NUM_LEDS; i++) {
    led_colors[i][0] = 0;
    led_colors[i][1] = 0;
    led_colors[i][2] = 255;
  }
}


void loop(){
  int comms_data = readData();
  if (comms_data > 0) {
    mode = comms_data;
    Serial.println(mode);
  }

  if (mode == 1) {
    brightness_current = breathe.turnOff(brightness_current);
  }
  else if (mode == 2) {
    brightness_current = breathe.updateBreathe(brightness_current);
  }
  else {
    brightness_current = 200;
    pulse.updatePulses();
  }
  fillLEDs();

  if (millis() - last_update > UPDATES_PER_SECOND) {
    leds.show();
    last_update = millis();
  }
}

int readData() {
  int data_out = 0;
  if(radio.available()){
    radio.read(&data_arr, sizeof(data_arr));
    data_out = atoi(data_arr);
  }
  return data_out;
}

void fillLEDs () {
  //Serial.println((String) led_colors[0][0] + ", " + led_colors[0][1] + ", " + led_colors[0][2]);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, leds.Color(led_colors[i][0], led_colors[i][1], led_colors[i][2]));
  }
  leds.setBrightness(brightness_current);
}