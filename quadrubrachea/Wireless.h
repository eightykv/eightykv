#include <SoftwareSerial.h>
#include <SPI.h>
#include <RF24.h>
#include "Arduino.h"
#include "Utility.h"

RF24 radio(7, 8); // CE, CSN
const byte read_address[6]  = "00001";
const byte write_address[6] = "00002";

char wireless_data[64];

void processData() {
  String data = String(wireless_data);
  String x_str = data.substring(0, 3);
  x = x_str.toInt();
  
  String y_str = data.substring(4, 7);
  y = y_str.toInt();
  
  String z_str = data.substring(8, 11);
  z = z_str.toInt();

  // Figure out which arms are active
  char i_str = data.charAt(12);
  char m_str = data.charAt(14);
  char r_str = data.charAt(16);
  char p_str = data.charAt(18);

  bool imrp[NUM_ARMS];
  imrp[0] = i_str == '1';
  imrp[1] = m_str == '1';
  imrp[2] = r_str == '1';
  imrp[3] = p_str == '1';

  for (int i = 0; i < NUM_ARMS; i++) {
    if (imrp[i] && !arm_on[i]) {
      arm_on_new[i] = true;
    }
    arm_on[i] = imrp[i];
  }
}

void setupWireless() {
  radio.begin();
  radio.openReadingPipe(0, read_address);
  radio.openWritingPipe(write_address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void readWireless() {
  if(radio.available()){
    radio.read(&wireless_data, sizeof(wireless_data));
    processData();
  }
}

void writeWireless(char *message) {
  radio.write(message, sizeof(*message));
}

