#ifndef GCODE_H
#define GCODE_H

#define WORD_A  0 // rotating axis 1
#define WORD_B  1 // rotating axis 2
#define WORD_C  2 // rotating axis 3
#define WORD_X  3 // linear axis 1
#define WORD_Y  4 // linear axis 2
#define WORD_Z  5 // linear axis 3
#define WORD_F  6 // feedrate
#define WORD_N  7 // line number
#define WORD_P  8 // dwell time, servo id
#define WORD_S  9 // servo position
#define WORD_G  10 // general function
#define WORD_M  11 // miscellaneous function

#define MOTION_RAPID 0
#define MOTION_LINEAR 1
#define DISTANCE_MODE_ABSOLUTE 90
#define DISTANCE_MODE_INCREMENTAL 91
#define UNITS_INCH 20
#define UNITS_MM 21
#define STEPPER_POWER_ON 17
#define STEPPER_POWER_OFF 18

struct modal {
  // G code modal groups from NIST spec:
  int motion; // G0, G1 (not used: G2, G3, G38.2, G80, G81, G82, G83, G84, G85, G86, G87, G88, G89) TODO use
  // not used: int plane_selection; // G17, G18, G19
  int distance_mode; // G90, G91
  // not used: int feed_rate_mode; // G93, G94
  int units; // G20, G21 TODO use
  // not used: int cutter_radius_compensation; // G40, G41, G42
  // not used: int tool_length_offset; // G43, G49
  // not used: int return_mode_in_canned_cycles; // G98, G99
  // not used: int coordinate system selection; // G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3
  // not used: int path control mode; // G61, G61.1, G64

  // M code modal groups from NIST spec:
  // not used: int stopping;
  // not used: int tool_change;
  // not used: int spindle_turning;
  // not used: int coolant;
  // not used: int feed_and_speed_override_switches;
};

struct state {
  struct modal modal;
  float position[6];
  float offset[6]; // TODO use it (and set by G92)
};

struct gcode_block {
  int g;
  int m;
  bool axis_words_used;
  int words;
  float values[WORD_S+1];
};

void parse(char *line, struct state *state, struct gcode_block *block);

#endif
