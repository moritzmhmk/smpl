#include "settings.h"

void load_settings(struct axis *axes) {
  axes[AXIS_A].used = true;
  axes[AXIS_A].stepper.en_pin = 8;
  axes[AXIS_A].stepper.dir_pin = 5;
  axes[AXIS_A].stepper.dir_inverted = true;
  axes[AXIS_A].stepper.step_pin = 2;
  axes[AXIS_A].stepper.steps_per_unit = 16*200;
  axes[AXIS_A].limit.pin = UNDEFINED_PIN;
  axes[AXIS_A].limit.inverted;
  axes[AXIS_A].home.pin = 9;
  axes[AXIS_A].home.inverted = false;
  axes[AXIS_A].home.step = 1;
  axes[AXIS_A].home.seekrate = 2000;
  axes[AXIS_A].home.feedrate = 500;
  axes[AXIS_A].home.dir = -1;

  axes[AXIS_B].used = true;
  axes[AXIS_B].stepper.en_pin = 8;
  axes[AXIS_B].stepper.dir_pin = 6;
  axes[AXIS_B].stepper.dir_inverted = false;
  axes[AXIS_B].stepper.step_pin = 3;
  axes[AXIS_B].stepper.steps_per_unit = 16*200;
  axes[AXIS_B].limit.pin = UNDEFINED_PIN;
  axes[AXIS_B].limit.inverted;
  axes[AXIS_B].home.pin = UNDEFINED_PIN;
//  axes[AXIS_B].home.inverted;
//  axes[AXIS_B].home.step;
//  axes[AXIS_B].home.seekrate;
//  axes[AXIS_B].home.feedrate;
//  axes[AXIS_B].home.dir;

  axes[AXIS_Z].used = true;
  axes[AXIS_Z].stepper.en_pin = 8;
  axes[AXIS_Z].stepper.dir_pin = 7;
  axes[AXIS_Z].stepper.dir_inverted = true;
  axes[AXIS_Z].stepper.step_pin = 4;
  axes[AXIS_Z].stepper.steps_per_unit = 1*200/3;
  axes[AXIS_Z].limit.pin = 11;
  axes[AXIS_Z].limit.inverted = false;
  axes[AXIS_Z].home.pin = 10;
  axes[AXIS_Z].home.inverted = true;
  axes[AXIS_Z].home.step = 1;
  axes[AXIS_Z].home.seekrate = 400;
  axes[AXIS_Z].home.feedrate = 200;
  axes[AXIS_Z].home.dir = -1;

  axes[AXIS_A].id = 'A';
  axes[AXIS_B].id = 'B';
  axes[AXIS_C].id = 'C';
  axes[AXIS_X].id = 'X';
  axes[AXIS_Y].id = 'Y';
  axes[AXIS_Z].id = 'Z';
}
