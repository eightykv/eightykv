// NRF communication
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

int talk_pin = 2;
int last_talk_val;
int talk_mode = 1;
long talk_retrig;

int onoff_pin = 3;
int last_onoff_val;
int onoff_mode = 0;
long onoff_retrig;

int retrig_amt = 200;

char output_arr[8];

void setup() {
  Serial.begin(115200);

  pinMode(talk_pin, INPUT_PULLUP);
  last_talk_val = digitalRead(talk_pin);
  talk_retrig = millis();

  pinMode(onoff_pin, INPUT_PULLUP);
  last_onoff_val = digitalRead(onoff_pin);
  onoff_retrig = millis();

  // Setup NRF
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();

}

void loop() {
  int talk_val = digitalRead(talk_pin);
  int new_mode = -1;
  if (onoff_mode == 1 && talk_val != last_talk_val && millis() - talk_retrig > retrig_amt) {
    last_talk_val = talk_val;
    talk_retrig = millis();
    talk_mode = 2 + (!talk_val);
    Serial.println((String) "talk: " + talk_mode);
    new_mode = talk_mode;
  }
  
  int onoff_val = digitalRead(onoff_pin);
  if (onoff_val != last_onoff_val && millis() - onoff_retrig > retrig_amt) {
    last_onoff_val = onoff_val;
    onoff_retrig = millis();
    onoff_mode = !onoff_val;
    Serial.println((String) "onoff: " + onoff_mode);
    new_mode = 1 + onoff_mode;
  }

  if (new_mode > -1) {
    sprintf(output_arr, "%01d", new_mode);
    Serial.println(output_arr);
    radio.write(&output_arr, sizeof(output_arr));
    delay(20);
  }
}
