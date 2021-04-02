#ifndef comms_h
#define comms_h

#include <SoftwareSerial.h>
#include <SPI.h>
#include <RF24.h>
const RF24 nrf(7, 8); // CE, CSN
const byte address[6] = "00001";

class Comms {
private:
  char data_arr[2];
  
public:
  Comms();
  int readData();
  
};

#endif
