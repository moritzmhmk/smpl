#ifndef SETTINGS_H
#define SETTINGS_H
#include "gcode.h"

struct trigger {
  int pin;
  bool inverted;
};

struct axis {
  char id;
  bool used;
  struct stepper {
    int en_pin;
    int dir_pin;
    bool dir_inverted;
    int step_pin;
    int steps_per_unit;
  } stepper;
  struct trigger limit;
  struct home: trigger {
    int step;
    int seekrate;
    int feedrate;
    bool dir;
  } home;
};

void load_settings(struct axis *axes);

#endif

// DO NOT CHANGE:
#define AXIS_A WORD_A
#define AXIS_B WORD_B
#define AXIS_C WORD_C
#define AXIS_X WORD_X
#define AXIS_Y WORD_Y
#define AXIS_Z WORD_Z

#define UNDEFINED_PIN -1
