#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();
int vals[12];

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(10);
  }
  
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
}

void loop() {
  
  for (uint8_t i=0; i<12; i++) {
    vals[i] = cap.baselineData(i) - cap.filteredData(i);
  }

  char b[64];
  sprintf(b, "%03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d", vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6], vals[7], vals[8], vals[9], vals[10], vals[11]);
  Serial.println(b);  
  
  // put a delay so it isn't overwhelming
  delay(100);
}
