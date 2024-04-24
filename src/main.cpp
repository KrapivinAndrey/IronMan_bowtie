// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN       3
#define BTN_PIN   4 

#define R 2
#define G 130
#define B 195

#define R_center 190
#define G_center 255
#define B_center 255

#define NUMPIXELS      7  

volatile bool intFlag = false;   // флаг
volatile uint8_t step = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int sin_color(int color, int step) {
  float Val=abs((sin(step*(3.1415/180))));
  return int(Val*color);
}

int cos_color(int color, int step) {
  float Val=abs((cos(step*(3.1415/180))));
  return int(Val*color);
}

void reactor(uint8_t wait) {
  for (int step=0; step<180;step++) {
    for (int i=0;i<NUMPIXELS-1;i++) {
      pixels.setPixelColor(i, pixels.Color(sin_color(R, step), sin_color(G, step), sin_color(B, step))); 
    }
    
    pixels.setPixelColor(NUMPIXELS-1, pixels.Color(cos_color(R_center, step), cos_color(G_center, step), cos_color(B_center, step))); 
    pixels.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

void sparks(uint8_t wait) {
  int num = random(7);
  pixels.clear();
  pixels.setPixelColor(num, R, G, B);
  pixels.show();
  delay(wait);
}

void loading(uint8_t wait) {

  pixels.clear();
  pixels.setBrightness(255);
  pixels.setPixelColor(1, 128, 128, 128);
  pixels.setPixelColor(2, 30, 30, 30);
  pixels.setPixelColor(3, 10, 10, 10);
  pixels.show();
  
}

void buttonTick() {
  intFlag = true;   // подняли флаг прерывания
}

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  pinMode(BTN_PIN, INPUT_PULLUP);
  attachInterrupt(0, buttonTick, FALLING);
}

void loop() {
  
  if (intFlag) {
    intFlag = false;    // сбрасываем
    // совершаем какие-то действия
    Serial.println("Interrupt!");
  }

  loading(250);

}

