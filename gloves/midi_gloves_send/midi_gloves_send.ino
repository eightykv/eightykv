#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055(55);

float x, y, z;
float prev_x, prev_y, prev_z;
float l_x, l_y, l_z;
float l_y_trig = 5.0;
bool l_y_bin = false;
int at = 0;
long ms;
int vox;

double diff = 35;
double p_diff = 30;
boolean calibrate;
double i_cal, m_cal, r_cal, p_cal;
double i, m, r, p, i_bin, m_bin, r_bin, p_bin, p_on = false;
int count = 100;
sensors_event_t event;
imu::Vector<3> lin_accel;

// Output data
byte binary = 0;
byte output_table[4] = {0x00, 0x10, 0x20, 0x30};
String output;
int x_out, y_out, z_out;
char output_arr[29];

void setup() {
  Serial.begin(57600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();
  
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    // Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  /* Display some basic information on this sensor */
  //displaySensorDetails();

  /* Optional: Display current status */
  //displaySensorStatus();

  bno.setExtCrystalUse(true);  
  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(4, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  calibrate = true;
}

void loop() {
  /* Get a new sensor event */
  if(count % 2 == 0) {
    bno.getEvent(&event);
    lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  }

  // Reset the binary values array
  binary = 0;
  output = "";
  
  l_x = abs(lin_accel.x());
  l_y = abs(lin_accel.y());
  l_z = abs(lin_accel.z());
  l_y_bin = l_y > l_y_trig;
  
  // Turn y on until I do this again
  if(l_y_bin) {
    if(!at && millis() - ms >= 500) {
      at = 127;
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      ms = millis();
    }
    else {
      if(millis() - ms >= 500) {
        at = 0;
        digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        ms = millis();
      }
    }
  }
    
  x_out = round((abs(180.0 - event.orientation.x)/180.0) * 127.0);
  x_out = x_out == 0 ? prev_x : x_out;
  x_out = x_out > 127.0 ? 127.0 : x_out;
  y_out = round(abs(event.orientation.y/90.0) * 127.0);
  y_out = y_out == 0 ? prev_y : y_out;
  y_out = y_out > 127.0 ? 127.0 : y_out;
  z_out = round(abs(event.orientation.z/90.0) * 127.0);
  z_out = z_out == 0 ? prev_z : z_out;
  z_out = z_out > 127.0 ? 127.0 : z_out;

  prev_x = x_out == 0 ? prev_x : x_out;
  prev_y = y_out == 0 ? prev_y : y_out;
  prev_z = z_out == 0 ? prev_z : z_out;

  /* Wait the specified delay before requesting nex data */
  
  i = (analogRead(A0));
  m = (analogRead(A1));
  r = (analogRead(A2));
  p = (analogRead(A3));
  
  double calVal[4];
  if(calibrate) {
    i_cal = i;
    m_cal = m;
    r_cal = r;
    p_cal = p;
    cal(calVal);
    calibrate = false;
  }
  
  i_bin = i < (i_cal - diff) ? 1.0 : 0.0;
  m_bin = m < (m_cal - diff) ? 1.0 : 0.0;
  r_bin = r < (r_cal - diff) ? 1.0 : 0.0;
  p_bin = p < (p_cal - p_diff) ? 1.0 : 0.0;
  
  int at_flag = 0;
  int i_flag = 0;
  int m_flag = 0;
  int r_flag = 0;
  int p_flag = 0;
  
  if(at)    { at_flag = 1; }
  if(i_bin) { i_flag = 1; }
  if(m_bin) { m_flag = 1; }
  if(r_bin) { r_flag = 1; }
  if(p_bin) { p_flag = 1; }
  
  int d3 = digitalRead(3);
  int d4 = digitalRead(4);
  int v_flag = 1;
  if (d3 == 0) {
    v_flag = 2;
  }
  if (d4 == 0) {
    v_flag = 0;
  }

  sprintf(output_arr, "%03d %03d %03d %d %d %d %d %d %d", x_out, y_out, z_out, i_flag, m_flag, r_flag, p_flag, at_flag, v_flag);
  Serial.println(output_arr);
  
  radio.write(&output_arr, sizeof(output_arr));
  
  count = count + 1;
  delay(20);
}

void cal(double* calVal) {
  for (int j = 0; j < 100; j++ ) {
    i_cal = (i_cal + analogRead(A0))/2;
    m_cal = (m_cal + analogRead(A1))/2;
    r_cal = (r_cal + analogRead(A2))/2;
    p_cal = /*568.0;*/(p_cal + analogRead(A3))/2;
    delay(10);
  }
  calibrate = false;
}
