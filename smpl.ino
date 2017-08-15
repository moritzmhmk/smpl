#include "settings.h"
#include "gcode.h"

struct axis axes[6];

void setup() {
  load_settings(axes);
  Serial.begin(9600);
  Serial.println("START");
  Serial.println(axes[AXIS_A].limit.pin);

  for(unsigned char i = 0; i < 6; i = i + 1) {
    pinMode(axes[i].stepper.en_pin, OUTPUT);
    pinMode(axes[i].stepper.dir_pin, OUTPUT);
    pinMode(axes[i].stepper.step_pin, OUTPUT);
    pinMode(axes[i].limit.pin, INPUT_PULLUP);
    pinMode(axes[i].home.pin, INPUT_PULLUP);
  }
}

struct state state;
struct gcode_block block;

void loop() {
  char line[64];
  int _l = Serial.readBytesUntil('\n', line, 64);

  line[_l] = '\0';
  if (_l) {
    Serial.print("[RECEIVED] ");
    Serial.println(line);
  }

  //reset block
  block = {0};
  parse(line, &state, &block);

  if(block.g == 4) {
    delay(block.values[WORD_P]); // TODO P not set
    Serial.println("Z_move_comp");
  }

  if(block.g == 28) {
    bool all = !bitRead(block.words, WORD_A) && !bitRead(block.words, WORD_B) && !bitRead(block.words, WORD_C) && !bitRead(block.words, WORD_X) && !bitRead(block.words, WORD_Y) && !bitRead(block.words, WORD_Z);
    if (all) {
      Serial.println("Homing all axes");
    } else {
      Serial.println("Homing selected axes");
    }
    for(unsigned char i = 0; i < 6; i = i + 1) {
      if (all || bitRead(block.words, i)) home(i);
    }
  }

  if(block.g == 92) { // G92 - set position
    for(unsigned char i = 0; i < 6; i = i + 1) {
      state.position[i] = bitRead(block.words, i) ? block.values[i] : state.position[i];
    }
  }

  if(!block.axis_words_used) { // no non modal gcode uses axis words -> movement
    float feedrate = bitRead(block.words, WORD_F) ? block.values[WORD_F] : 100; //TODO define default feedrate
    for(unsigned char i = 0; i < 6; i = i + 1) {
      float d = 0;
      if (state.modal.distance_mode == DISTANCE_MODE_INCREMENTAL) {
        d = block.values[i]*bitRead(block.words, i);
      } else {
        d = bitRead(block.words, i) ? block.values[i] - state.position[i] : 0;
      }
      if (d) move_axis(i, abs(d), d<0, feedrate);
      state.position[i] += d;
    }
  }

  if(block.m == 17) { // enable steppers
    for(unsigned char i = 0; i < 6; i = i + 1) {
      digitalWrite(axes[i].stepper.en_pin, LOW);
    }
  }
  if(block.m == 18) { // disable steppers
    for(unsigned char i = 0; i < 6; i = i + 1) {
      digitalWrite(axes[i].stepper.en_pin, HIGH);
    }
  }

  if(block.m == 114) {
    for(unsigned char i = 0; i < 6; i = i + 1) {
      Serial.print(axes[i].id);
      Serial.print(":");
      Serial.print(state.position[i]);
      Serial.print(" ");
    }
    Serial.println();
  }

  if(block.m == 119) {
    Serial.print("limit: ");
    for(unsigned char i = 0; i < 6; i = i + 1) {
      if (!axes[i].used || axes[i].limit.pin == UNDEFINED_PIN) continue;
      Serial.print(axes[i].id);
      Serial.print(isTriggered(axes[i].limit));
      Serial.print(" ");
    }
    Serial.print("; home: ");
    for(unsigned char i = 0; i < 6; i = i + 1) {
      if (!axes[i].used || axes[i].home.pin == UNDEFINED_PIN) continue;
      Serial.print(axes[i].id);
      Serial.print(isTriggered(axes[i].home));
      Serial.print(" ");
    }
    Serial.println();
  }
}

bool isTriggered(struct trigger t) {
  return t.pin != UNDEFINED_PIN && digitalRead(t.pin) ^ t.inverted;
}

void home(char axis) {
  if (!axes[axis].used || axes[axis].home.pin == UNDEFINED_PIN) return;
  float step = axes[axis].home.step;
  float seekrate = axes[axis].home.seekrate;
  float feedrate = axes[axis].home.feedrate;
  int dir = axes[axis].home.dir;
  Serial.println("Search switch:");
  while (isTriggered(axes[axis].home)) move_axis(axis, step, dir, seekrate);
  Serial.println("Retract from switch (slow):");
  while (!isTriggered(axes[axis].home)) move_axis(axis, step, !dir, feedrate);
  Serial.println("Retract further:");
  move_axis(axis, 2 * step, !dir, seekrate);
  Serial.println("Search switch (slow):");
  while (isTriggered(axes[axis].home)) move_axis(axis, step/2, dir, feedrate);
}

void move_axis(char axis, float units, bool dir, float feedrate) {
  if (!axes[axis].used) return;
  Serial.print("[INFO] moving axis "); Serial.print(axes[axis].id); Serial.print(" "); Serial.print(units); Serial.println("units.");

  int dir_pin = axes[axis].stepper.dir_pin;
  bool dir_inverted = axes[axis].stepper.dir_inverted;
  int step_pin = axes[axis].stepper.step_pin;
  float steps_per_unit = axes[axis].stepper.steps_per_unit;

  if (dir_pin!=UNDEFINED_PIN) {
    int steps = round(units * steps_per_unit);
    digitalWrite(dir_pin, dir ^ dir_inverted);
    for (int i=0; i<steps; i++) {
      if (isTriggered(axes[axis].limit)) {
        Serial.println("[ERROR] limit switch hit!"); // TODO how to handle properly?
        return;
      }
      digitalWrite(step_pin, HIGH);
      delay(60 * 500 / (feedrate * steps_per_unit));
      digitalWrite(step_pin, LOW);
      delay(60 * 500 / (feedrate * steps_per_unit));
    }
  }
}
