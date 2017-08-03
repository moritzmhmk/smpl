#define A  0 // rotating axis 1
#define B  1 // rotating axis 2
#define C  2 // rotating axis 3
#define F  3 // feedrate
#define G  4 // general function
#define M  5 // miscellaneous function
#define N  6 // line number
#define P  7 // dwell time, servo id
#define S  8 // servo position
#define X  9 // linear axis 1
#define Y  10 // linear axis 2
#define Z  11 // linear axis 3

#define PIN_EN 8

#define PIN_A_DIR 5
#define PIN_A_STEP 2
#define PIN_B_DIR 6
#define PIN_B_STEP 3
// #define PIN_C_DIR 7
// #define PIN_C_STEP 4
// #define PIN_X_DIR 5
// #define PIN_X_STEP 2
// #define PIN_Y_DIR 6
// #define PIN_Y_STEP 3
#define PIN_Z_DIR 7
#define PIN_Z_STEP 4

// #define PIN_LIMIT_A 9
// #define PIN_LIMIT_B 10
// #define PIN_LIMIT_C 11
// #define PIN_LIMIT_X 9
// #define PIN_LIMIT_Y 10
#define PIN_LIMIT_Z 11
#define LIMIT_Z_INVERT 0

#define PIN_HOME_A 9
// #define PIN_HOME_B 10
// #define PIN_HOME_C 11
// #define PIN_HOME_X 9
// #define PIN_HOME_Y 10
#define PIN_HOME_Z 10


#define A_STEPS_PER_REV (16*200)
#define B_STEPS_PER_REV (16*200)
// #define C_STEPS_PER_REV (16*200)
// #define X_STEPS_PER_MM 0
// #define Y_STEPS_PER_MM 0
#define Z_STEPS_PER_MM (1*200/3)

#define A_DIR_INVERT 1
#define B_DIR_INVERT 0
#define C_DIR_INVERT 0
#define X_DIR_INVERT 0
#define Y_DIR_INVERT 0
#define Z_DIR_INVERT 1

#define A_HOME_STEP 1
#define A_HOME_SEEKRATE 2000
#define A_HOME_FEEDRATE 500
#define A_HOME_DIR -1

#define Z_HOME_STEP 1
#define Z_HOME_SEEKRATE 400
#define Z_HOME_FEEDRATE 200
#define Z_HOME_DIR -1
#define Z_HOME_PIN_INVERT 1

float pos_A, pos_B, pos_C, pos_X, pos_Y, pos_Z;
bool relative_movement;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_EN, OUTPUT);

  #ifdef PIN_A_DIR
  pinMode(PIN_A_DIR, OUTPUT);
  #endif
  #ifdef PIN_B_DIR
  pinMode(PIN_B_DIR, OUTPUT);
  #endif
  #ifdef PIN_C_DIR
  pinMode(PIN_C_DIR, OUTPUT);
  #endif
  #ifdef PIN_X_DIR
  pinMode(PIN_X_DIR, OUTPUT);
  #endif
  #ifdef PIN_Y_DIR
  pinMode(PIN_Y_DIR, OUTPUT);
  #endif
  #ifdef PIN_Z_DIR
  pinMode(PIN_Z_DIR, OUTPUT);
  #endif

  #ifdef PIN_A_STEP
  pinMode(PIN_A_STEP, OUTPUT);
  #endif
  #ifdef PIN_B_STEP
  pinMode(PIN_B_STEP, OUTPUT);
  #endif
  #ifdef PIN_C_STEP
  pinMode(PIN_C_STEP, OUTPUT);
  #endif
  #ifdef PIN_X_STEP
  pinMode(PIN_X_STEP, OUTPUT);
  #endif
  #ifdef PIN_Y_STEP
  pinMode(PIN_Y_STEP, OUTPUT);
  #endif
  #ifdef PIN_Z_STEP
  pinMode(PIN_Z_STEP, OUTPUT);
  #endif

  #ifdef PIN_LIMIT_A
  pinMode(PIN_LIMIT_A, INPUT_PULLUP);
  #endif
  #ifdef PIN_LIMIT_B
  pinMode(PIN_LIMIT_B, INPUT_PULLUP);
  #endif
  #ifdef PIN_LIMIT_C
  pinMode(PIN_LIMIT_C, INPUT_PULLUP);
  #endif
  #ifdef PIN_LIMIT_X
  pinMode(PIN_LIMIT_X, INPUT_PULLUP);
  #endif
  #ifdef PIN_LIMIT_Y
  pinMode(PIN_LIMIT_Y, INPUT_PULLUP);
  #endif
  #ifdef PIN_LIMIT_Z
  pinMode(PIN_LIMIT_Z, INPUT_PULLUP);
  #endif

  #ifdef PIN_HOME_A
  pinMode(PIN_HOME_A, INPUT_PULLUP);
  #endif
  #ifdef PIN_HOME_B
  pinMode(PIN_HOME_B, INPUT_PULLUP);
  #endif
  #ifdef PIN_HOME_C
  pinMode(PIN_HOME_C, INPUT_PULLUP);
  #endif
  #ifdef PIN_HOME_X
  pinMode(PIN_HOME_X, INPUT_PULLUP);
  #endif
  #ifdef PIN_HOME_Y
  pinMode(PIN_HOME_Y, INPUT_PULLUP);
  #endif
  #ifdef PIN_HOME_Z
  pinMode(PIN_HOME_Z, INPUT_PULLUP);
  #endif
}

void loop() {
  char line[64];
  int _l = Serial.readBytesUntil('\n', line, 64);

  if (_l) {
    line[_l] = '\0';
    Serial.println(line);
  }

  int i = 0;
  int found = 0;
  float value[Z+1];
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
  }

  if(bitRead(found, G) && value[G] == 0) {
    float new_A, new_B, new_C, new_X, new_Y, new_Z, feedrate;
    if (relative_movement) {
      new_A = pos_A + value[A]*bitRead(found, A);
      new_B = pos_B + value[B]*bitRead(found, B);
      new_C = pos_C + value[C]*bitRead(found, C);
      new_X = pos_X + value[X]*bitRead(found, X);
      new_Y = pos_Y + value[Y]*bitRead(found, Y);
      new_Z = pos_Z + value[Z]*bitRead(found, Z);
    } else {
      new_A = bitRead(found, A) ? value[A] : pos_A;
      new_B = bitRead(found, B) ? value[B] : pos_B;
      new_C = bitRead(found, C) ? value[C] : pos_C;
      new_X = bitRead(found, X) ? value[X] : pos_X;
      new_Y = bitRead(found, Y) ? value[Y] : pos_Y;
      new_Z = bitRead(found, Z) ? value[Z] : pos_Z;
    }
    feedrate = bitRead(found, F) ? value[F] : 100; //TODO define default feedrate
    move_axes(new_A-pos_A, new_B-pos_B, new_C-pos_C, new_X-pos_X, new_Y-pos_Y, new_Z-pos_Z, feedrate);
    pos_A=new_A;
    pos_B=new_B;
    pos_C=new_C;
    pos_X=new_X;
    pos_Y=new_Y;
    pos_Z=new_Z;
  }

  if(bitRead(found, G) && value[G] == 4) {
    delay(value[P]);
    Serial.println("Z_move_comp");
  }

  if(bitRead(found, G) && value[G] == 28) {
    bool all = !bitRead(found, A) && !bitRead(found, B) && !bitRead(found, C) && !bitRead(found, X) && !bitRead(found, Y) && !bitRead(found, Z);
    if (all) {
      Serial.println("Homing all axes");
    } else {
      Serial.println("Homing selected axes");
    }
    #ifdef PIN_HOME_A
     if (all || bitRead(found, A)) home('A');
    #endif
    #ifdef PIN_HOME_B
      if (all || bitRead(found, B)) home('B');
    #endif
    #ifdef PIN_HOME_C
      if (all || bitRead(found, C)) home('C');
    #endif
    #ifdef PIN_HOME_X
      if (all || bitRead(found, X)) home('X');
    #endif
    #ifdef PIN_HOME_Y
      if (all || bitRead(found, Y)) home('Y');
    #endif
    #ifdef PIN_HOME_Z
      if (all || bitRead(found, Z)) home('Z');
    #endif
  }

  if(bitRead(found, G) && value[G] == 90) relative_movement = false;
  if(bitRead(found, G) && value[G] == 91) relative_movement = true;
  if(bitRead(found, G) && value[G] == 92) { // G92 - set position
    pos_A = bitRead(found, A) ? value[A] : pos_A;
    pos_B = bitRead(found, B) ? value[B] : pos_B;
    pos_C = bitRead(found, C) ? value[C] : pos_C;
    pos_X = bitRead(found, X) ? value[X] : pos_X;
    pos_Y = bitRead(found, Y) ? value[Y] : pos_Y;
    pos_Z = bitRead(found, Z) ? value[Z] : pos_Z;
  }

  if(bitRead(found, M) && value[M] == 17) digitalWrite(PIN_EN, LOW); // enable steppers
  if(bitRead(found, M) && value[M] == 18) digitalWrite(PIN_EN, HIGH); // disable steppers

  if(bitRead(found, M) && value[M] == 114) {
    Serial.print("A:");Serial.print(pos_A);
    Serial.print(" B:");Serial.print(pos_B);
    Serial.print(" C:");Serial.print(pos_C);
    Serial.print(" X:");Serial.print(pos_X);
    Serial.print(" Y:");Serial.print(pos_Y);
    Serial.print(" Z:");Serial.println(pos_Z);
  }

  if(bitRead(found, M) && value[M] == 119) {
    Serial.print("limit: ");
    #ifdef PIN_LIMIT_A
    Serial.print("A");
    Serial.print(digitalRead(PIN_LIMIT_A));
    #endif
    #ifdef PIN_LIMIT_B
    Serial.print("B");
    Serial.print(digitalRead(PIN_LIMIT_B));
    #endif
    #ifdef PIN_LIMIT_C
    Serial.print("C");
    Serial.print(digitalRead(PIN_LIMIT_C));
    #endif
    #ifdef PIN_LIMIT_X
    Serial.print("X");
    Serial.print(digitalRead(PIN_LIMIT_X));
    #endif
    #ifdef PIN_LIMIT_Y
    Serial.print("Y");
    Serial.print(digitalRead(PIN_LIMIT_Y));
    #endif
    #ifdef PIN_LIMIT_Z
    Serial.print("Z");
    Serial.print(digitalRead(PIN_LIMIT_Z));
    #endif
    Serial.print("; home: ");
    #ifdef PIN_HOME_A
    Serial.print("A");
    Serial.print(digitalRead(PIN_HOME_A));
    #endif
    #ifdef PIN_HOME_B
    Serial.print("B");
    Serial.print(digitalRead(PIN_HOME_B));
    #endif
    #ifdef PIN_HOME_C
    Serial.print("C");
    Serial.print(digitalRead(PIN_HOME_C));
    #endif
    #ifdef PIN_HOME_X
    Serial.print("X");
    Serial.print(digitalRead(PIN_HOME_X));
    #endif
    #ifdef PIN_HOME_Y
    Serial.print("Y");
    Serial.print(digitalRead(PIN_HOME_Y));
    #endif
    #ifdef PIN_HOME_Z
    Serial.print("Z");
    Serial.print(digitalRead(PIN_HOME_Z));
    #endif
    Serial.println();
  }
}

void home(char axis) {
  int pin;
  bool invert = false;
  float step, seekrate, feedrate;
  int dir;
  switch (axis) {
    #if defined(PIN_HOME_A) && defined(A_HOME_STEP) && defined(A_HOME_SEEKRATE) && defined(A_HOME_FEEDRATE) && defined(A_HOME_DIR)
    case 'A':
      pin = PIN_HOME_A;
      #ifdef A_HOME_PIN_INVERT
      invert = A_HOME_PIN_INVERT;
      #endif
      step = A_HOME_STEP;
      seekrate = A_HOME_SEEKRATE;
      feedrate = A_HOME_FEEDRATE;
      dir = A_HOME_DIR;
      break;
    #endif
    #if defined(PIN_HOME_B) && defined(B_HOME_STEP) && defined(B_HOME_SEEKRATE) && defined(B_HOME_FEEDRATE) && defined(B_HOME_DIR)
    case 'B':
      pin = PIN_HOME_B;
      #ifdef B_HOME_PIN_INVERT
      invert = B_HOME_PIN_INVERT;
      #endif
      step = B_HOME_STEP;
      seekrate = B_HOME_SEEKRATE;
      feedrate = B_HOME_FEEDRATE;
      dir = B_HOME_DIR;
      break;
    #endif
    #if defined(PIN_HOME_C) && defined(C_HOME_STEP) && defined(C_HOME_SEEKRATE) && defined(C_HOME_FEEDRATE) && defined(C_HOME_DIR)
    case 'C':
      pin = PIN_HOME_C;
      #ifdef C_HOME_PIN_INVERT
      invert = C_HOME_PIN_INVERT;
      #endif
      step = C_HOME_STEP;
      seekrate = C_HOME_SEEKRATE;
      feedrate = C_HOME_FEEDRATE;
      dir = C_HOME_DIR;
      break;
    #endif
    #if defined(PIN_HOME_X) && defined(X_HOME_STEP) && defined(X_HOME_SEEKRATE) && defined(X_HOME_FEEDRATE) && defined(X_HOME_DIR)
    case 'X':
      pin = PIN_HOME_X;
      #ifdef X_HOME_PIN_INVERT
      invert = X_HOME_PIN_INVERT;
      #endif
      step = X_HOME_STEP;
      seekrate = X_HOME_SEEKRATE;
      feedrate = X_HOME_FEEDRATE;
      dir = X_HOME_DIR;
      break;
    #endif
    #if defined(PIN_HOME_Y) && defined(Y_HOME_STEP) && defined(Y_HOME_SEEKRATE) && defined(Y_HOME_FEEDRATE) && defined(Y_HOME_DIR)
    case 'Y':
      pin = PIN_HOME_Y;
      #ifdef Y_HOME_PIN_INVERT
      invert = Y_HOME_PIN_INVERT;
      #endif
      step = Y_HOME_STEP;
      seekrate = Y_HOME_SEEKRATE;
      feedrate = Y_HOME_FEEDRATE;
      dir = Y_HOME_DIR;
      break;
    #endif
    #if defined(PIN_HOME_Z) && defined(Z_HOME_STEP) && defined(Z_HOME_SEEKRATE) && defined(Z_HOME_FEEDRATE) && defined(Z_HOME_DIR)
    case 'Z':
      pin = PIN_HOME_Z;
      #ifdef Z_HOME_PIN_INVERT
      invert = Z_HOME_PIN_INVERT;
      #endif
      step = Z_HOME_STEP;
      seekrate = Z_HOME_SEEKRATE;
      feedrate = Z_HOME_FEEDRATE;
      dir = Z_HOME_DIR;
      break;
    #endif
  }
  Serial.println("Search switch:");
  while (digitalRead(pin) ^ invert) move_axis(axis, step * dir, seekrate);
  Serial.println("Retract from switch (slow):");
  while (!digitalRead(pin) ^ invert) move_axis(axis, -step * dir, feedrate);
  Serial.println("Retract further:");
  move_axis(axis, -2 * step * dir, seekrate);
  Serial.println("Search switch (slow):");
  while (digitalRead(pin) ^ invert) move_axis(axis, step/2 * dir, feedrate);
}

void move_axes(float dA, float dB, float dC, float dX, float dY, float dZ, float feedrate) {
  Serial.print("move");
  Serial.print(" A:");Serial.print(dA);
  Serial.print("\tB:");Serial.print(dB);
  Serial.print("\tC:");Serial.print(dC);
  Serial.print("\tX:");Serial.print(dX);
  Serial.print("\tY:");Serial.print(dY);
  Serial.print("\tZ:");Serial.println(dZ);
  if (dA) move_axis('A', dA, feedrate);
  if (dB) move_axis('B', dB, feedrate);
  if (dC) move_axis('C', dC, feedrate);
  if (dX) move_axis('X', dX, feedrate);
  if (dY) move_axis('Y', dY, feedrate);
  if (dZ) move_axis('Z', dZ, feedrate);
}

void move_axis(char axis, float units, float feedrate) {
  int dir_pin = -1;
  int step_pin = -1;
  int limit_pin = -1;
  bool limit_invert = false;
  bool dir_invert = false;
  float steps_per_unit;
  switch (axis) {
    #if defined(PIN_A_DIR) && defined(PIN_A_STEP) && defined(A_STEPS_PER_REV)
    case 'A':
      dir_pin = PIN_A_DIR;
      step_pin = PIN_A_STEP;
      steps_per_unit = A_STEPS_PER_REV / 360.0;
      dir_invert = A_DIR_INVERT;
      #ifdef PIN_LIMIT_A
      limit_pin = PIN_LIMIT_A;
      #endif
      #ifdef LIMIT_A_INVERT
      limit_invert = LIMIT_A_INVERT;
      #endif
      break;
    #endif
    #if defined(PIN_B_DIR) && defined(PIN_B_STEP) && defined(B_STEPS_PER_REV)
    case 'B':
      dir_pin = PIN_B_DIR;
      step_pin = PIN_B_STEP;
      steps_per_unit = B_STEPS_PER_REV / 360.0;
      dir_invert = B_DIR_INVERT;
      #ifdef PIN_LIMIT_B
      limit_pin = PIN_LIMIT_B;
      #endif
      #ifdef LIMIT_B_INVERT
      limit_invert = LIMIT_B_INVERT;
      #endif
      break;
    #endif
    #if defined(PIN_C_DIR) && defined(PIN_C_STEP) && defined(C_STEPS_PER_REV)
    case 'C':
      dir_pin = PIN_C_DIR;
      step_pin = PIN_C_STEP;
      steps_per_unit = C_STEPS_PER_REV / 360.0;
      dir_invert = C_DIR_INVERT;
      #ifdef PIN_LIMIT_C
      limit_pin = PIN_LIMIT_C;
      #endif
      #ifdef LIMIT_C_INVERT
      limit_invert = LIMIT_C_INVERT;
      #endif
      break;
    #endif
    #if defined(PIN_X_DIR) && defined(PIN_X_STEP) && defined(X_STEPS_PER_MM)
    case 'X':
      dir_pin = PIN_X_DIR;
      step_pin = PIN_X_STEP;
      steps_per_unit = X_STEPS_PER_MM;
      dir_invert = X_DIR_INVERT;
      #ifdef PIN_LIMIT_X
      limit_pin = PIN_LIMIT_X;
      #endif
      #ifdef LIMIT_X_INVERT
      limit_invert = LIMIT_X_INVERT;
      #endif
      break;
    #endif
    #if defined(PIN_Y_DIR) && defined(PIN_Y_STEP) && defined(Y_STEPS_PER_MM)
    case 'Y':
      dir_pin = PIN_Y_DIR;
      step_pin = PIN_Y_STEP;
      steps_per_unit = Y_STEPS_PER_MM;
      dir_invert = Y_DIR_INVERT;
      #ifdef PIN_LIMIT_Y
      limit_pin = PIN_LIMIT_Y;
      #endif
      #ifdef LIMIT_Y_INVERT
      limit_invert = LIMIT_Y_INVERT;
      #endif
      break;
    #endif
    #if defined(PIN_Z_DIR) && defined(PIN_Z_STEP) && defined(Z_STEPS_PER_MM)
    case 'Z':
      dir_pin = PIN_Z_DIR;
      step_pin = PIN_Z_STEP;
      steps_per_unit = Z_STEPS_PER_MM;
      dir_invert = Z_DIR_INVERT;
      #ifdef PIN_LIMIT_Z
      limit_pin = PIN_LIMIT_Z;
      #endif
      #ifdef LIMIT_Z_INVERT
      limit_invert = LIMIT_Z_INVERT;
      #endif
      break;
    #endif
  }
  if (dir_pin>=0) {
    int steps = round(abs(units) * steps_per_unit);
    bool dir = (units < 0) ^ dir_invert;
    digitalWrite(dir_pin, dir);
    for (int i=0; i<steps; i++) {
      if (limit_pin >= 0 && digitalRead(limit_pin) ^ limit_invert) {
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
      return A;
    case 'b':
    case 'B':
      return B;
    case 'c':
    case 'C':
      return C;
    case 'f':
    case 'F':
      return F;
    case 'g':
    case 'G':
      return G;
    case 'm':
    case 'M':
      return M;
    case 'n':
    case 'N':
      return N;
    case 'p':
    case 'P':
      return P;
    case 's':
    case 'S':
      return S;
    case 'x':
    case 'X':
      return X;
    case 'y':
    case 'Y':
      return Y;
    case 'z':
    case 'Z':
      return Z;
     default:
      return -1;
  }
}