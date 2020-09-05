#include <Adafruit_NeoPixel.h>

const int NUM_STRIPS = 4;
const int LED_PIN[NUM_STRIPS] = {2, 3, 4, 5};
const int LED_COUNT = 51;

Adafruit_NeoPixel strip0(LED_COUNT, LED_PIN[0], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1(LED_COUNT, LED_PIN[1], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_COUNT, LED_PIN[2], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3(LED_COUNT, LED_PIN[3], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel *lights[NUM_STRIPS] = {&strip0, &strip1, &strip2, &strip3};

struct Pulse {
  int offset; // Maximum color offset from baseline -- only in green
  int actual_offsets[32];
  int width; // How large this pulse is. Stored as half of the total width
  int pulse_start; // The first pixel (lowest index) to have its color changed by this pulse
  int pulse_end; // The first pixel (lowest index) to have its color changed by this pulse
  bool move_direction; // true if up, false if down
  int move_delay; // How long to wait before moving to the next pixel
  long last_move; // The last time this moved
  bool active; // If this pulse is inactive, it'll be overwritten
};

Pulse *pulses[7];

//const int LED_FORMAT = NEO_GRB + NEO_KHZ800;
const int DEFAULT_COLOR[3] = {255, 0, 0};
const int TIME_BETWEEN = 1000;
// Width is between one third and one tenth of the length of the strip
const int MIN_WIDTH = 5;
const int MAX_WIDTH = 16;
// Max color offset
const int MIN_OFFSET = 3;
const int MAX_OFFSET = 16;
// Min and max delay
const int MIN_DELAY = 30;
const int MAX_DELAY = 300;
// Max number of pulses
const int MAX_PULSES = 7;
int num_pulses = 0;
long last_pulse;

void setup() {
  Serial.begin(57600);
  randomSeed(analogRead(A0));

  for (int i = 0; i < NUM_STRIPS; i++) {
    lights[i]->begin();
    lights[i]->setBrightness(128);
    for (int j = 0; j < LED_COUNT; j++) {
      lights[i]->setPixelColor(j, lights[i]->Color(DEFAULT_COLOR[0], DEFAULT_COLOR[1], DEFAULT_COLOR[2]));
    }
    lights[i]->show();
  }
  
  // put your setup code here, to run once:
  for (int i = 0; i < MAX_PULSES; i++) {
    pulses[i] = new Pulse();
  }

  last_pulse = millis();
}

void loop() {
  for (int i = 0; i < NUM_STRIPS; i++) {
    updatePulses();
    updatePixels(i);
  }
  delay(10);
}

void updatePixels(int which_strip) {
  Adafruit_NeoPixel *light = lights[which_strip];
  
  for (int i = 0; i < LED_COUNT; i++) {
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
    light->setPixelColor(i, light->Color(new_color[0], new_color[1], new_color[2]));
  }

  light->show();
}

void updatePulses() {
  long m = millis();
  
  // Randomly decide whether to initialize a new pulse
  if (m - last_pulse > TIME_BETWEEN && num_pulses < 7) {
    
    float prob = (7 - num_pulses) / 7.0;
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
        if ((dir && p->pulse_start >= LED_COUNT) || 
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
}

Pulse *newPulse(int index) {
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
    p->pulse_start = LED_COUNT - 1;
    p->pulse_end = LED_COUNT - 2 + (p->width * 2);
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

float randFloat() {
  return random(0, 100) / 99.0;
}
