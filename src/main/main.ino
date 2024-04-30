// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <helpers.h>
#include <const.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN       3
#define BTN_PIN   2 


volatile bool intFlag = false;   // флаг
volatile uint16_t draw_step = 0;
volatile uint16_t draw_mode = 0;

#define REACTOR 0
#define RAINBOW 1
#define SPARKS 2
#define LOADING 3


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


#pragma region Effects

void reactor() {

  for (int i=0; i<NUMPIXELS-1;i++) {

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

  draw_step%=180;
  delay(WAIT_REACTOR);

}

void rainbow() {

  for( int i=0; i < NUMPIXELS; i++) {

    pixels.setPixelColor(i, Wheel((draw_step+i*10) & 255));
  }

  pixels.show();

  draw_step%=255;
  delay(WAIT_RAINBOW);
  
}

void sparks() {

  int num = random(7);

  pixels.clear();
  pixels.setPixelColor(num, R_REACTOR, G_REACTOR, B_REACTOR);
  pixels.show();

  draw_step = 0;

  delay(WAIT_SPARKS);

}

void loading() {

  pixels.clear();
  pixels.setPixelColor((draw_step + 1) % 6, 128, 128, 128);
  pixels.setPixelColor((draw_step + 2) % 6, 30, 30, 30);
  pixels.setPixelColor((draw_step + 3) % 6, 10, 10, 10);

  draw_step%=6;

  pixels.show();

  delay(WAIT_LOADING);
  
}

#pragma endregion Effects

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
    draw_step = 0;
    draw_mode = (draw_mode + 1) % 4;
  }

  switch (draw_mode)
  {
  case REACTOR: reactor(); break;
  case RAINBOW: rainbow(); break;
  case SPARKS: sparks(); break;
  case LOADING: loading(); break;
  }

  draw_step++;

}

