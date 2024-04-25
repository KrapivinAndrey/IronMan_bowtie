// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <Effects.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN       3
#define BTN_PIN   2 

#define R_REACTOR 2
#define G_REACTOR 130
#define B_REACTOR 195

#define R_REACTOR_CENTER 190
#define G_REACTOR_CENTER 255
#define B_REACTOR_CENTER 255

#define WAIT_REACTOR 25

#define NUMPIXELS      7  

volatile bool intFlag = false;   // флаг
volatile int draw_step = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);



void reactor() {

  for (int i=0;i<NUMPIXELS-1;i++) {

    pixels.setPixelColor(i, 
                          pixels.Color(sin_color(R_REACTOR, draw_step), 
                          sin_color(G_REACTOR, draw_step), 
                          sin_color(B_REACTOR, draw_step))); 
  }
    
  pixels.setPixelColor(NUMPIXELS-1, 
                          pixels.Color(cos_color(R_REACTOR_CENTER, draw_step), 
                          cos_color(G_REACTOR_CENTER, draw_step), 
                          cos_color(B_REACTOR_CENTER, draw_step))); 
  pixels.show();

  delay(WAIT_REACTOR);

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
  pixels.setPixelColor(num, R_REACTOR, G_REACTOR, B_REACTOR);
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
  Serial.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.
  pinMode(BTN_PIN, INPUT_PULLUP);
  attachInterrupt(0, buttonTick, FALLING);
}

void loop() {
  
  if (intFlag) {
    intFlag = false;    // сбрасываем
    // совершаем какие-то действия
    Serial.println("flash");
    draw_step = 0;
  }

  reactor();
  draw_step++;

}

