#include <MIDI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Servo.h>

#define BNO055_SAMPLERATE_DELAY_MS (100);
Adafruit_BNO055 bno = Adafruit_BNO055(55);
MIDI_CREATE_DEFAULT_INSTANCE();
Servo pendulum;

double x, y, z;
double x1, y1, z1;
double x_max = -2048.0, y_max = -2048.0, z_max = -2048.0;
double x_min = 2048.0, y_min = 2048.0, z_min = 2048.0;
double x_out, y_out, z_out;
double l_x, l_y, l_z;
double g_x, g_y, g_z;
double high = 1.5;
int pos = 95, zero = 95, h = 145, l = 45, change = 1;
bool plus = true, sentOnce = false;
int on = 0, tics = 0, count = 0;

void setup() {
  Serial.begin(9600);
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    // Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  pendulum.attach(9);  // attaches the servo on pin 9 to the servo object

  bno.setExtCrystalUse(true); 
}

void loop() {
  on = digitalRead(8);
  if(on == HIGH) {
    sentOnce = false;
    if(tics == 0) {
      sensors_event_t event;
      bno.getEvent(&event);
    
      x = abs(180 - event.orientation.x) / 180;//fmod(abs(((180 - event.orientation.x) / 180) * 255) * 2, 255);
      if(x > x_max) {
        x_max = x;
      }
      if(x < x_min) {
        x_min = x;
      }
      x_out = 127 - calculate_out(x, x_min, x_max);
      y = abs(event.orientation.y / 90);
      if(y > y_max) {
        y_max = y;
      }
      if(y < y_min) {
        y_min = y;
      }
      y_out = calculate_out(y, y_min, y_max);
      z = abs(event.orientation.z / 90);
      if(z > z_max) {
        z_max = z;
      }
      if(z < z_min) {
        z_min = z;
      }
      z_out = calculate_out(z, z_min, z_max);
      /*Serial.print(x_out);
      Serial.print(", ");
      Serial.print(y_out);
      Serial.print(", ");
      Serial.println(z);*/
      
      imu::Vector<3> lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
      l_x = abs(lin_accel.x());
      l_y = abs(lin_accel.y());
      l_z = abs(lin_accel.z());
      
      imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
      g_x = abs(gyro.x());
      g_y = abs(gyro.y());
      g_z = abs(gyro.z());
    
      int x_filtered = round((g_x / high) * 127);
      x_filtered = x_filtered > 127 ? 127 : x_filtered;
      int y_filtered = round((g_y / high) * 127);
      y_filtered = y_filtered > 127 ? 127 : y_filtered;
      int z_filtered = round((g_z / high) * 127);
      z_filtered = z_filtered > 127 ? 127 : z_filtered;
      //MIDI.sendPitchBend(calculate_bend(y_filtered), 1);
      byte cc_x = x_out;
      MIDI.sendControlChange(16, x_out, 1);
      byte cc_y = y_out;
      MIDI.sendControlChange(17, y_out, 1);
      byte cc_z = z_out;
      MIDI.sendControlChange(18, z_out, 1);
    
      if(abs(y_filtered - y1) > 30 && count > 10) {
        count = 0;
      }
    
      x1 = x_filtered;
      y1 = y_filtered;
      z1 = z_filtered;
    
      digitalWrite(13, 1);
      count = count + 1;
    }
  
    if(plus && pos < h) {
      pos = pos + change;
      pendulum.write(pos);
    }
    else if(plus && pos >= h) {
      MIDI.sendNoteOn(30, 127, 1);
      plus = false;
      pos = pos - change;
      pendulum.write(pos);
    }
    else if(!plus && pos > l) {
      pos = pos - change;
      pendulum.write(pos);
    }
    else if(!plus && pos <= l) {
      plus = true;
      pos = pos + change;
      pendulum.write(pos);
    }
  
    tics = (tics + 10) % 100;
  }
  else {
    if(!sentOnce) {
      pendulum.write(zero);
      MIDI.sendNoteOff(30, 127, 1);
      sentOnce = true;
    }
  }
  delay(10);
}

int calculate_out(double val, double v_min, double v_max) {
  double percent_val = (val - v_min) / v_max;
  double calc_val_1 = percent_val * 127;
  int calc_val_2 = max(0, calc_val_1);
  int calc_val_3 = min(127, calc_val_2);
  return calc_val_3;
}

int calculate_bend(int val) {
  double percent = val / 127.0;
  int total_distance = abs(MIDI_PITCHBEND_MIN) + MIDI_PITCHBEND_MAX;
  int newVal = (percent * total_distance) - MIDI_PITCHBEND_MAX;
  return newVal;
}

