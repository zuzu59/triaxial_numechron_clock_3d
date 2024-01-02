//zf240102.1614
// License CC BY-NC-SA
// Hide-away unit case for Arduino nano and 28BYJ-48 stepper motor
// by shiura is licensed under the Creative Commons - Attribution - Non-Commercial - Share Alike license. 
// https://www.thingiverse.com/thing:5145523
// https://www.thingiverse.com/shiura/designs

// Tune the following value if the clock gains or loses. Positive value slows down your clock.
// ex. if your clock gains 100seconds per 24hours, this value should be 100.
#define COMPENSATION_SECONDS 0

// If COMPENSATION_SECONDS is zero, this value is 60000 = 60 seconds
#define MILLIS_PER_MIN (60000L + (25L * (COMPENSATION_SECONDS)) / 36L)

// ports used to control the stepper motor
// if your motor rotate to the opposite direction, 
// change the order as {4, 5, 6, 7};
// for Arduino nano
int port[4] = {7, 6, 5, 4};
// for ESP32 (example)
// int port[4] = {12, 32, 25, 27};

// Motor and clock parameters
#define STEPS_PER_ROTATION 1536 // steps for a minute

// wait for a single step of stepper
int delaytime = 3;

// sequence of stepper motor control
int seq[4][4] = {
  {  LOW,  LOW, HIGH,  LOW},
  {  LOW,  LOW,  LOW, HIGH},
  { HIGH,  LOW,  LOW,  LOW},
  {  LOW, HIGH,  LOW,  LOW}
};

void rotate(int step) {
  static int phase = 0;
  int i, j;
  int delta = (step > 0) ? 1 : 3;
  int dt = 20;

  step = (step > 0) ? step : -step;
  for(j = 0; j < step; j++) {
    phase = (phase + delta) % 4;
    for(i = 0; i < 4; i++) {
      digitalWrite(port[i], seq[phase][i]);
    }
    delay(dt);
    if(dt > delaytime) dt--;
  }
  // power cut
  for(i = 0; i < 4; i++) {
    digitalWrite(port[i], LOW);
  }
}

void setup() {
  pinMode(port[0], OUTPUT);
  pinMode(port[1], OUTPUT);
  pinMode(port[2], OUTPUT);
  pinMode(port[3], OUTPUT);
  rotate(-10); // for approach run
  rotate(10); // approach run without heavy load
  rotate(STEPS_PER_ROTATION);
}

void loop() {
  static long prev_min = 0, prev_pos = 0;
  long min;
  static long pos;
  
  min = millis() / MILLIS_PER_MIN;
  if(prev_min == min) {
    return;
  }
  prev_min = min;
  pos = (STEPS_PER_ROTATION * min);
  rotate(-10); // for approach run
  rotate(10); // approach run without heavy load
  if(pos - prev_pos > 0) {
    rotate(pos - prev_pos);
  }
  prev_pos = pos;
}
