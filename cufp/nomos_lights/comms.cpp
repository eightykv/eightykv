#include "comms.h"

Comms::Comms() {
  Serial.println("before begin");
  nrf.begin();
  nrf.openReadingPipe(0, address);
  nrf.setPALevel(RF24_PA_MIN);
  nrf.startListening();
}

int Comms::readData() {
  int new_mode = -1;
  // Read from NRF
  if(nrf.available()){
    Serial.println("before read");
    nrf.read(&data_arr, sizeof(data_arr));
    Serial.println("after read");
    new_mode = atoi(data_arr);
  }
  return new_mode;
}
