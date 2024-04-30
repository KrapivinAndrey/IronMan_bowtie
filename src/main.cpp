// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <FastLED.h>
CRGBPalette16 gPal;


#ifdef __AVR__
  #include <avr/power.h>
#endif

#pragma region Const
#define R_REACTOR 2
#define G_REACTOR 130
#define B_REACTOR 195

#define R_REACTOR_CENTER 190
#define G_REACTOR_CENTER 255
#define B_REACTOR_CENTER 255

// настройки пламени
#define HUE_START 3     // начальный цвет огня (0 красный, 80 зелёный, 140 молния, 190 розовый)
#define HUE_GAP 18      // коэффициент цвета огня (чем больше - тем дальше заброс по цвету)
#define SMOOTH_K 0.15   // коэффициент плавности огня
#define MIN_BRIGHT 80   // мин. яркость огня
#define MAX_BRIGHT 255  // макс. яркость огня
#define MIN_SAT 245     // мин. насыщенность
#define MAX_SAT 255     // макс. насыщенность

// ****************************** ОГОНЬ ******************************
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


#define WAIT_REACTOR 30
#define WAIT_RAINBOW 30
#define WAIT_SPARKS 60
#define WAIT_LOADING 90
#define WAIT_FIRE 90

#define NUMPIXELS 7 
#pragma endregion

// для разработчиков
#define ZONE_AMOUNT NUMPIXELS   // количество зон
byte zoneValues[ZONE_AMOUNT];
byte zoneRndValues[ZONE_AMOUNT];

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN       3
#define BTN_PIN   2 


volatile bool intFlag = false;   // флаг
volatile uint16_t draw_step = 0;
volatile uint16_t draw_mode = 0;
volatile uint32_t prevTime;

#define REACTOR 0
#define RAINBOW 1
#define SPARKS 2
#define LOADING 3
#define FIRE 4


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#pragma region Helpers

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

uint32_t HSV(byte h, byte s, byte v) {
  byte r, g, b;
	byte value = ((24 * h / 17) / 60) % 6;
	byte vmin = (long)v - v * s / 255;
	byte a = (long)v * s / 255 * (h * 24 / 17 % 60) / 60;
	byte vinc = vmin + a;
	byte vdec = v - a;
	switch (value) {
	case 0: r = v; g = vinc; b = vmin; break;
	case 1: r = vdec; g = v; b = vmin; break;
	case 2: r = vmin; g = v; b = vinc; break;
	case 3: r = vmin; g = vdec; b = v; break;
	case 4: r = vinc; g = vmin; b = v; break;
	case 5: r = v; g = vmin; b = vdec; break;
	}
	return Color(r, g, b);
}

void Fire2012WithPalette()
{
  // Array of temperature readings at each simulation cell
  static byte heat[3];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < 3; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / 3) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = 3 - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors

  // layer 0
  byte colorindex = scale8(heat[2], 240);
  CRGB pixel = ColorFromPalette(gPal, colorindex);
  pixels.setPixelColor(1, pixel.r, pixel.g, pixel.b);
  pixels.setPixelColor(2, pixel.r, pixel.g, pixel.b);

  // layer 1
  colorindex = scale8(heat[1], 240);
  pixel = ColorFromPalette(gPal, colorindex);
  pixels.setPixelColor(0, pixel.r, pixel.g, pixel.b);
  pixels.setPixelColor(6, pixel.r, pixel.g, pixel.b);
  pixels.setPixelColor(3, pixel.r, pixel.g, pixel.b);

  // layer 2
  colorindex = scale8(heat[0], 240);
  pixel = ColorFromPalette(gPal, colorindex);
  pixels.setPixelColor(5, pixel.r, pixel.g, pixel.b);
  pixels.setPixelColor(4, pixel.r, pixel.g, pixel.b);
}

uint32_t getFireColor(int val) {
  // чем больше val, тем сильнее сдвигается цвет, падает насыщеность и растёт яркость
  return HSV(
           HUE_START + map(val, 20, 60, 0, HUE_GAP),                    // H
           constrain(map(val, 20, 60, MAX_SAT, MIN_SAT), 0, 255),       // S
           constrain(map(val, 20, 60, MIN_BRIGHT, MAX_BRIGHT), 0, 255)  // V
          );
}

#pragma endregion

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

    pixels.setPixelColor(i, Wheel((draw_step+i*30) & 255));
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

void fire() {
  if (millis() - prevTime > WAIT_FIRE) {
    prevTime = millis();
    random16_add_entropy( random());
    Fire2012WithPalette(); // run simulation frame, using palette colors
    pixels.show();
  }
}

#pragma endregion Effects

void buttonTick() {
  intFlag = true;   // подняли флаг прерывания
}

void setup() {
  gPal = HeatColors_p;
  pixels.begin(); // This initializes the NeoPixel library.
  pinMode(BTN_PIN, INPUT_PULLUP);
  attachInterrupt(0, buttonTick, FALLING);
}

void loop() {
  
  if (intFlag) {
    intFlag = false;    // сбрасываем
    // совершаем какие-то действия
    draw_step = 0;
    draw_mode = (draw_mode + 1) % 5;
  }

  // switch (draw_mode)
  // {
  // case REACTOR: reactor(); break;
  // case RAINBOW: rainbow(); break;
  // case SPARKS: sparks(); break;
  // case LOADING: loading(); break;
  // case FIRE: fire(); break;
  // }
  fire();

  draw_step++;

}
