#include "settings.h"

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

float pos[6];
int linear_interpolation = false; // unset by G0; set by G1 TODO currently unused
bool metric_units = true; // unset by G20; set by G21 TODO currently unused
bool incremental_movement = false; // unset by G90; set by G91

void loop() {
  char line[64];
  int _l = Serial.readBytesUntil('\n', line, 64);

  if (_l) {
    line[_l] = '\0';
    Serial.println(line);
  }

  int i = 0;
  int found = 0;
  float value[WORD_S+1];
  int non_modal_gcode = -1;
  while (i<_l) {
    char c = line[i++];
    if (c==' ') continue;
    int w = toWordCode(c);
    if (w==-1) {
      Serial.print("Character \"");
      Serial.print(c);
      Serial.println("\" ignored");
      continue;
    }
    bitSet(found, w);
    value[w] = readFloat(line, &i);
    // multiple G codes are allowed per line (one per modal group)
    // relevant modes will be set here
    if (w == WORD_G) {
      switch((int)value[w]) {
        case 0: linear_interpolation = false; break;
        case 1: linear_interpolation = true; break;
        case 20: metric_units = false; break;
        case 21: metric_units = true; break;
        case 90: incremental_movement = false; break;
        case 91: incremental_movement = true; break;
        case 4:
        case 28:
        case 92: non_modal_gcode = value[w]; break;
      }
    }
  }

  if(non_modal_gcode == 4) {
    delay(value[WORD_P]); // TODO P not set
    Serial.println("Z_move_comp");
  }

  if(non_modal_gcode == 28) {
    bool all = !bitRead(found, WORD_A) && !bitRead(found, WORD_B) && !bitRead(found, WORD_C) && !bitRead(found, WORD_X) && !bitRead(found, WORD_Y) && !bitRead(found, WORD_Z);
    if (all) {
      Serial.println("Homing all axes");
    } else {
      Serial.println("Homing selected axes");
    }
    for(unsigned char i = 0; i < 6; i = i + 1) {
      if (all || bitRead(found, i)) home(i);
    }
  }

  if(non_modal_gcode == 92) { // G92 - set position
    for(unsigned char i = 0; i < 6; i = i + 1) {
      pos[i] = bitRead(found, i) ? value[i] : pos[i];
    }
  }

  if(non_modal_gcode != 28 && non_modal_gcode != 92) { // no code that uses axis words -> movement
    float feedrate = bitRead(found, WORD_F) ? value[WORD_F] : 100; //TODO define default feedrate
    for(unsigned char i = 0; i < 6; i = i + 1) {
      float d = 0;
      if (incremental_movement) {
        d = value[i]*bitRead(found, i);
      } else {
        d = bitRead(found, i) ? value[i] - pos[i] : 0;
      }
      if (d) move_axis(i, abs(d), d<0, feedrate);
      pos[i] += d;
    }
  }

  if(bitRead(found, WORD_M) && value[WORD_M] == 17) { // enable steppers
    for(unsigned char i = 0; i < 6; i = i + 1) {
      digitalWrite(axes[i].stepper.en_pin, LOW);
    }
  }
  if(bitRead(found, WORD_M) && value[WORD_M] == 18) { // disable steppers
    for(unsigned char i = 0; i < 6; i = i + 1) {
      digitalWrite(axes[i].stepper.en_pin, HIGH);
    }
  }

  if(bitRead(found, WORD_M) && value[WORD_M] == 114) {
    for(unsigned char i = 0; i < 6; i = i + 1) {
      Serial.print(axes[i].id);
      Serial.print(":");
      Serial.print(pos[i]);
      Serial.print(" ");
    }
    Serial.println();
  }

  if(bitRead(found, WORD_M) && value[WORD_M] == 119) {
    Serial.print("limit: ");
    for(unsigned char i = 0; i < 6; i = i + 1) {
      if (!axes[i].used || axes[i].limit.pin == UNDEFINED_PIN) continue;
      Serial.print(axes[i].id);
      Serial.print(digitalRead(axes[i].limit.pin));
      Serial.print(" ");
    }
    Serial.print("; home: ");
    for(unsigned char i = 0; i < 6; i = i + 1) {
      if (!axes[i].used || axes[i].home.pin == UNDEFINED_PIN) continue;
      Serial.print(axes[i].id);
      Serial.print(digitalRead(axes[i].home.pin));
      Serial.print(" ");
    }
    Serial.println();
  }
}

void home(char axis) {
  if (!axes[axis].used || axes[axis].home.pin == UNDEFINED_PIN) return;
  int pin = axes[axis].home.pin;
  bool inverted = axes[axis].home.inverted;
  float step = axes[axis].home.step;
  float seekrate = axes[axis].home.seekrate;
  float feedrate = axes[axis].home.feedrate;
  int dir = axes[axis].home.dir;
  Serial.println("Search switch:");
  while (digitalRead(pin) ^ inverted) move_axis(axis, step, dir, seekrate);
  Serial.println("Retract from switch (slow):");
  while (!digitalRead(pin) ^ inverted) move_axis(axis, step, !dir, feedrate);
  Serial.println("Retract further:");
  move_axis(axis, 2 * step, !dir, seekrate);
  Serial.println("Search switch (slow):");
  while (digitalRead(pin) ^ inverted) move_axis(axis, step/2, dir, feedrate);
}

void move_axis(char axis, float units, bool dir, float feedrate) {
  if (!axes[axis].used) return;

  int dir_pin = axes[axis].stepper.dir_pin;
  bool dir_inverted = axes[axis].stepper.dir_inverted;
  int step_pin = axes[axis].stepper.step_pin;
  float steps_per_unit = axes[axis].stepper.steps_per_unit;

  int limit_pin = axes[axis].limit.pin;
  bool limit_inverted = axes[axis].limit.inverted;

  if (dir_pin!=UNDEFINED_PIN) {
    int steps = round(units * steps_per_unit);
    digitalWrite(dir_pin, dir ^ dir_inverted);
    for (int i=0; i<steps; i++) {
      if (limit_pin != UNDEFINED_PIN && digitalRead(limit_pin) ^ limit_inverted) {
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

float readFloat(char *line, int *i) {
  float f = 0;
  float fraction = 1;
  bool negative = false;
  char c = line[*i];
  if (c == '+') { *i = *i+1; }
  if (c == '-') {
    negative = true;
    *i = *i+1;
  }
  while (c != '\0') {
   c = line[*i];
   if(c>='0' && c<='9') {
    if (fraction == 1) {
      f *= 10;
    }
    f += (c - '0') * fraction;
    if (fraction < 1) {
      fraction /= 10;
    }
   } else if (c == '.' && fraction == 1) {
    fraction = 0.1;
   } else {
    return negative ? -f : f;
   }
   *i = *i+1;
  }
}

int toWordCode(char c) {
  switch (c) {
    case 'a':
    case 'A':
      return WORD_A;
    case 'b':
    case 'B':
      return WORD_B;
    case 'c':
    case 'C':
      return WORD_C;
    case 'f':
    case 'F':
      return WORD_F;
    case 'g':
    case 'G':
      return WORD_G;
    case 'm':
    case 'M':
      return WORD_M;
    case 'n':
    case 'N':
      return WORD_N;
    case 'p':
    case 'P':
      return WORD_P;
    case 's':
    case 'S':
      return WORD_S;
    case 'x':
    case 'X':
      return WORD_X;
    case 'y':
    case 'Y':
      return WORD_Y;
    case 'z':
    case 'Z':
      return WORD_Z;
     default:
      return -1;
  }
}
