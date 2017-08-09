#ifndef SETTINGS_H
#define SETTINGS_H

struct axis {
  char id;
  bool used;
  struct stepper {
    int en_pin;
    int dir_pin;
    bool dir_inverted;
    int step_pin;
    int steps_per_unit;
  }stepper;
  struct limit {
    int pin;
    bool inverted;
  }limit;
  struct home {
    int pin;
    bool inverted;
    int step;
    int seekrate;
    int feedrate;
    int dir;
  }home;
};

void load_settings(struct axis *axes);

#endif

// DO NOT CHANGE:
#define WORD_A  0 // rotating axis 1
#define WORD_B  1 // rotating axis 2
#define WORD_C  2 // rotating axis 3
#define WORD_X  3 // linear axis 1
#define WORD_Y  4 // linear axis 2
#define WORD_Z  5 // linear axis 3
#define WORD_F  6 // feedrate
#define WORD_G  7 // general function
#define WORD_M  8 // miscellaneous function
#define WORD_N  9 // line number
#define WORD_P  10 // dwell time, servo id
#define WORD_S  11 // servo position

#define AXIS_A WORD_A
#define AXIS_B WORD_B
#define AXIS_C WORD_C
#define AXIS_X WORD_X
#define AXIS_Y WORD_Y
#define AXIS_Z WORD_Z

#define UNDEFINED_PIN -1
