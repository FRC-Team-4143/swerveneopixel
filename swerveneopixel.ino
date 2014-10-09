#include <Adafruit_NeoPixel.h>
#include <Wire.h>

#define PIN 6

#define RED strip.Color(100,0,0)
#define GREEN strip.Color(0,100,0)
#define BLUE strip.Color(0,0,100)
#define OFF strip.Color(0,0,0)
#define BLANKS 39
#define SIDELEN 40

#define STRIPLEN 160

#define I2CADDR 0x4

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIPLEN, PIN, NEO_GRB + NEO_KHZ800);

uint8_t goti2c = 0;
uint8_t pattern[16] = {0,0,0,0,0,20,60,100,150,255,255,255,150,100,60,20};
uint8_t place = 0;

void setup() {
  strip.begin();
  Wire.begin(I2CADDR);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  Serial.print("setup\n\r");
  strip.show(); // Initialize all pixels to 'off'
  colorWipe(RED, 0);
  colorWipe(OFF,0);  
}

void fillpattern () {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(pattern[(i+place)%16],0,0));
  }
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  //colorWipe(strip.Color(255, 0, 0), 50); // Red
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  //colorWipe(strip.Color(0, 0, 255), 50); // Blue
  //rainbow(20);
  //rainbowCycle(20);

  if(goti2c == 1) {
    delay(100);
    return;
  }

  for(uint8_t i=0; i<5; i++) {
      fillpattern();
      strip.show();
      place++;
      delay(10);
      if(goti2c == 1) return;
   }
   
   delay(300);
   if(goti2c == 1) return;

  for(uint8_t i=0; i<5; i++) {
      fillpattern();
      strip.show();
      place++;
      delay(10);
      if(goti2c == 1) return;
   }

   delay(1000);
}

void receiveEvent(int howMany)
{
  goti2c = 1;

  char c = 0;
  while(1 < Wire.available()) // loop through all but the last
  {
    c = Wire.read(); // receive byte as a character
    Serial.println(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
  if(c == 1)   
    colorWipe(BLUE, 0);
  else
    frontback(x);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void frontback(uint8_t start) {
  uint8_t i, j;
  
  while(start > SIDELEN*2)
      start -= SIDELEN*2;
  
  if(start < SIDELEN) {
      for(j=0,i=0; j<BLANKS; j++)
          strip.setPixelColor(i++,OFF);
      for(j=0; j<start; j++)
          strip.setPixelColor(i++,RED);
      for(j=0;j<SIDELEN; j++)
          strip.setPixelColor(i++,GREEN);
      for(j=0;j<SIDELEN; j++)
          strip.setPixelColor(i++,RED);
      for(j=0;j<SIDELEN; j++)
          strip.setPixelColor(i++,GREEN);
  }
  else {
      start -= SIDELEN;
      for(j=0,i=0; j<BLANKS; j++)
          strip.setPixelColor(i++,OFF);
      for(j=0; j<start; j++)
          strip.setPixelColor(i++,GREEN);
      for(j=0;j<SIDELEN; j++)
          strip.setPixelColor(i++,RED);
      for(j=0;j<SIDELEN; j++)
          strip.setPixelColor(i++,GREEN);
      for(j=0;j<SIDELEN; j++)
          strip.setPixelColor(i++,RED);
  }
  strip.show();
}


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

