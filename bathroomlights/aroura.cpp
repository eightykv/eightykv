#include "aroura.h"

Aroura::Aroura(int _ndx) {
  ndx = _ndx;
  visible = false;
}

void Aroura::initialize(int base_color) {
  width = random(MIN_WIDTH, MAX_WIDTH);
  // Width always needs to be even
  if (width % 2 == 1) {
    width++;
  }

  // if it's outside of the sweet spot...
  if (base_color > 8000 || base_color < 3000) {
    // Use the much larger change values
    change = random(MIN_CHANGE, MAX_CHANGE);
    if (random(2)) {
      change = 0 - change;
    }
  }
  else {
    // Otherwise use the much subtler values
    change = random(ALT_MIN_CHANGE, ALT_MAX_CHANGE);
  }
  change_per = change / width;
  update_ms = random(MAX_SPEED, MIN_SPEED);
  dir = random(2);
  start_location = 0 - (width * 2);
  if (dir) {
    start_location = NUM_LEDS;
  }
  last_update = millis();
  visible = true;
  /*
  Serial.print("Initialize aroura ");
  Serial.println(ndx);
  
  
  Serial.print("Width: ");
  Serial.println(width);
  Serial.print("Change: ");
  Serial.println(change);
  Serial.print("Change per LED: ");
  Serial.println(change_per);
  Serial.print("Speed: ");
  Serial.println(update_ms);
  Serial.print("Dir: ");
  Serial.println(dir);
  Serial.print("Start location: ");
  Serial.println(start_location);*/
}

void Aroura::update_aroura() {
  if (!visible) {
    return;
  }
  
  if (millis() - last_update >= update_ms) {
    if (dir) {
      start_location--;
      if (start_location < (0 - (width * 2))) {
        finish();
        //Serial.print("Aroura finished: ");
        //Serial.println(ndx);
      }
    }
    else {
      start_location++;
      if (start_location > NUM_LEDS) {
        finish();
        //.print("Aroura finished: ");
        //Serial.println(ndx);
      }
    }
    last_update = millis();
  }
  
}

void Aroura::finish() {
  visible = false;
}

bool Aroura::is_on() {
  return visible;
}

int Aroura::get_start() {
  return start_location;
}

int Aroura::get_width() {
  return width;
}

long Aroura::get_change(int loc) {
  long return_val = change_per * loc;
  if (loc >= width) {
    return_val = change_per * (width - (loc - width) - 1);
  }
  return return_val;
}
