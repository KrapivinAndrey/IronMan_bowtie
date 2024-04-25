#include <Arduino.h>

int sin_color(int color, int step) {
  float Val = abs((sin(step*(3.1415/180))));
  return int(Val*color);
}

int cos_color(int color, int step) {
  float Val = abs((cos(step*(3.1415/180))));
  return int(Val*color);
}
