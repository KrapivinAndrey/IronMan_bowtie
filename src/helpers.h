#include <Arduino.h>

static uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
  }

int sin_color(int color, int step) {
  float Val = abs((sin(step*(3.1415/180))));
  return int(Val*color);
}

int cos_color(int color, int step) {
  float Val = abs((cos(step*(3.1415/180))));
  return int(Val*color);
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}