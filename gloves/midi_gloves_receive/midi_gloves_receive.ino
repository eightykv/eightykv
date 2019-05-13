#include <SoftwareSerial.h>
#include <MIDI.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

//SoftwareSerial mySerial(3, 2); //RX, TX
MIDI_CREATE_DEFAULT_INSTANCE();
String data;
bool stop_read = false;
char datum[3];
int x, y, z;
int _x, _y, _z, _on;
float f_x, f_y, f_z;
int on, i, m, r, p, i_on, m_on, r_on, p_on;
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
  /*
  if(data.charAt(0) == 'x' && data.charAt(6) == 'y' && data.charAt(12) == 'z' && data.charAt(18) == 'f') {
    Serial.println(data);
    
    // x data
    datum[0] = data.charAt(2);
    datum[1] = data.charAt(3);
    datum[2] = data.charAt(4);
    x = atoi(datum);
    f_x = float(x);
    f_x = (abs(180.0 - f_x) / 180.0) * 127.0;
    _x = round(f_x);
    if(f_x > 127) { _x = 127; }
    
    // y data
    datum[0] = data.charAt(8);
    datum[1] = data.charAt(9);
    datum[2] = data.charAt(10);
    y = atoi(datum);
    f_y = (float(y) / 90.0) * 127.0;
    _y = round(f_y);
    if(f_y > 127) { _y = 127; }
    
    // z data
    datum[0] = data.charAt(14);
    datum[1] = data.charAt(15);
    datum[2] = data.charAt(16);
    z = atoi(datum);
    f_z = (float(z) / 90.0) * 127.0;
    _z = round(f_z);
    if(f_z > 127) { _z = 127; }
    
    if(data.charAt(20) == '1') { on = 1; _on = 127; }
    else { on = 0; _on = 0; }
    
    MIDI.sendControlChange(19, _x, 1);
    MIDI.sendControlChange(20, _y, 1);
    MIDI.sendControlChange(21, _z, 1);
    MIDI.sendControlChange(22, _on, 1);

    if(data.charAt(21) == '1') { i = 1; }
    else { i = 0; }
    if(data.charAt(22) == '1') { m = 1; }
    else { m = 0; }
    if(data.charAt(23) == '1') { r = 1; }
    else { r = 0; }
    if(data.charAt(24) == '1') { p = 1; }
    else { p = 0; }
    
    if(i && !i_on) {
      MIDI.sendNoteOn(30, 127, 1);
      MIDI.sendControlChange(23, 127, 1);
      i_on = true;
    } else if(i == 0.0 && i_on) {
      MIDI.sendNoteOff(30, 127, 1);
      MIDI.sendControlChange(23, 0, 1);
      i_on = false;
    }
    if(m && !m_on) {
      MIDI.sendNoteOn(31, 127, 1);
      MIDI.sendControlChange(24, 127, 1);
      m_on = true;
    } else if(m == 0.0 && m_on) {
      MIDI.sendNoteOff(31, 127, 1);
      MIDI.sendControlChange(24, 0, 1);
      m_on = false;
    }
    if(r && !r_on) {
      MIDI.sendNoteOn(32, 127, 1);
      MIDI.sendControlChange(25, 127, 1);
      r_on = true;
    } else if(r == 0.0 && r_on) {
      MIDI.sendNoteOff(32, 127, 1);
      MIDI.sendControlChange(25, 0, 1);
      r_on = false;
    }
    if(p && !p_on) {
      MIDI.sendNoteOn(33, 127, 1);
      MIDI.sendControlChange(26, 127, 1);
      p_on = true;
    } else if(p == 0.0 && p_on) {
      MIDI.sendNoteOff(33, 127, 1);
      MIDI.sendControlChange(26, 0, 1);
      p_on = false;
    }
  }
  else {
  }*/
  delay(20);
}
