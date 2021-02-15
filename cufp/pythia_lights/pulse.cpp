#include "pulse.h"

PulseClass::PulseClass() {
  for (int i = 0; i < MAX_PULSES; i++) {
    pulses[i] = new Pulse();
  }
  
  last_pulse = millis();
}

void PulseClass::updatePixels() {
  for (int i = 0; i < NUM_LEDS; i++) {
    int new_color[3] = {DEFAULT_COLOR[0], DEFAULT_COLOR[1], DEFAULT_COLOR[2]};
    for (int j = 0; j < MAX_PULSES; j++) {
      Pulse *p = pulses[j];
      if (p->active && i >= p->pulse_start && i <= p->pulse_end) {
        // Calculate index relative to the end position of the pulse
        int relative_to = p->pulse_end - i;
        // Subtract relative position from total width
        int loc = ((p->width * 2) - 1) - relative_to;
        // Only changing the green value
        new_color[1] += p->offset;
      }
    }
    for (int j = 0; j < 3; j++) {
      led_colors[i][j] = new_color[j];
    }
  }
}

void PulseClass::updatePulses() {
  long m = millis();
  
  // Randomly decide whether to initialize a new pulse
  if (m - last_pulse > TIME_BETWEEN && num_pulses < MAX_PULSES) {
    
    float prob = (MAX_PULSES - num_pulses) / (float) MAX_PULSES;
    if (randFloat() < prob) {
      // Debug printout
      /*
      Serial.print("Adding pulse. Current pulses: ");
      Serial.println(num_pulses + 1);*/
      
      int i = 0;
      while (pulses[i]->active) {
        i++;
      }
      pulses[i] = newPulse(i);
      num_pulses++;
    }
    last_pulse = m;
  }

  // For each existing pulse...
  for (int i = 0; i < MAX_PULSES; i++) {
    Pulse *p = pulses[i];
    if (p->active) {
      bool dir = p->move_direction;
  
      // Check if we need to advance this pulse
      if (m - p->last_move > p->move_delay) {
        if (dir) {
          /*
          Serial.print(i);
          Serial.print(": ");
          Serial.println(p->pulse_start);*/
          p->pulse_start++;
          p->pulse_end++;
        }
        else {/*
          Serial.print(i);
          Serial.print(": ");
          Serial.println(p->pulse_start);*/
          p->pulse_start--;
          p->pulse_end--;
        }
        p->last_move = m;
      
        // Check if it still applies (if it's fully run off the edge, delete it)
        if ((dir && p->pulse_start >= NUM_LEDS) || 
             !dir && p->pulse_end <= 0) {
          p->active = false;
          num_pulses--;
          num_pulses = max(0, num_pulses);
          // Debug printout
          /*
          Serial.print("Removing pulse ");
          Serial.println(i);
          Serial.print("Pulses remaining: ");
          Serial.println(num_pulses);*/
        }
      }
    }
  }

  updatePixels();
}

Pulse *PulseClass::newPulse(int index) {
  Pulse *p = pulses[index];

  p->active = true;

  // Calculate color offset
  p->offset = random(MIN_OFFSET, MAX_OFFSET);
  
  // Calculate width
  p->width = ceil((MIN_WIDTH + (MAX_WIDTH - MIN_WIDTH) * randFloat())/2);

  // Calculate the _actual_ offsets for each pixel
  for (int w = 0; w < p->width; w++) {
    int perc = ceil(((w + 1) / (float)p->width) * p->offset);
    p->actual_offsets[w] = perc;
  }

  // Calculate start and end location based on move direction.
  // Always starts/ends so exactly one pixel with an offset is showing.
  p->move_direction = randFloat() < 0.66 ? true : false;
  // If we're going up, start below 0 and end at 0
  if (p->move_direction) {
    p->pulse_start = 1 - (p->width * 2);
    p->pulse_end = 0;
  }
  // If we're going down, start at the end of the string and end further above it
  if (!p->move_direction) {
    p->pulse_start = NUM_LEDS - 1;
    p->pulse_end = NUM_LEDS - 2 + (p->width * 2);
  }

  p->move_delay = random(MIN_DELAY, MAX_DELAY);
  
  p->last_move = millis();

  // Debug printout
  /*
  Serial.print("Offset: ");
  Serial.println(p->offset);
  Serial.print("Width: ");
  Serial.println(p->width);
  Serial.print("Direction: ");
  Serial.println(p->move_direction);
  Serial.print("Pulse start: ");
  Serial.println(p->pulse_start);
  Serial.print("Pulse end: ");
  Serial.println(p->pulse_end);
  Serial.print("Delay: ");
  Serial.println(p->move_delay);
  Serial.println();*/
  
  return p;
}

float PulseClass::randFloat() {
  return random(0, 100) / 99.0;
}
