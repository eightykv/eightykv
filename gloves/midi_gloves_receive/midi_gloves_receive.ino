#include <SoftwareSerial.h>
#include <SPI.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

String data;
char data_arr[32];

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if(radio.available()){
    radio.read(&data_arr, sizeof(data_arr));
    data = String(data_arr);
    Serial.println(data);
  }
  delay(20);
}