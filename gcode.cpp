#include "gcode.h"
#include "Arduino.h"

int toWordCode(char **c);
float readFloat(char **c);

void parse(char *c, struct state *state, struct gcode_block *block) {
  while (*c != '\0') {
    while (*c == ' ') ++c;
    int w = readWordCode(&c);
    if (w==-1) {
      Serial.print("[WARNING] unknown word letter \"");
      Serial.print(*c);
      Serial.println("\" ignored!");
      continue;
    }
    float v = readFloat(&c);
    if (w == WORD_G) {
      switch((int)v) {
        case 0: state->modal.motion = MOTION_RAPID; break;
        case 1: state->modal.motion = MOTION_LINEAR; break;

        case 20: state->modal.units = UNITS_INCH; break;
        case 21: state->modal.units = UNITS_MM; break;

        case 90: state->modal.distance_mode = DISTANCE_MODE_ABSOLUTE; break;
        case 91: state->modal.distance_mode = DISTANCE_MODE_INCREMENTAL; break;

        case 28:
        case 92: block->axis_words_used = true; // no break!
        case 4: block->g = v; break;

        default: Serial.print("[WARNING] ignoring unsupported command G"); Serial.println((int)v);
      }
    } else if (w == WORD_M) {
      block->m = v;
    } else {
      bitSet(block->words, w);
      block->values[w] = v;
    }
  }
}

int readWordCode(char **c) {
  switch (*(*c++)) {
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

float readFloat(char **c) {
  float f = 0;
  float fraction = 1;
  bool negative = false;
  if (**c == '+') { ++*c; }
  if (**c == '-') {
    negative = true;
    ++*c;
  }
  while (**c != '\0') {
    if(**c>='0' && **c<='9') {
      if (fraction == 1) {
        f *= 10;
      }
      f += (**c - '0') * fraction;
      if (fraction < 1) {
        fraction /= 10;
      }
    } else if (**c == '.' && fraction == 1) {
      fraction = 0.1;
    } else {
      break;
    }
    ++*c;
  }
  return negative ? -f : f;
}
