#include <Adafruit_NeoPixel.h>
#include <Wire.h>

#define PIN 6

#define RED strip.Color(100,0,0)
#define GREEN strip.Color(0,100,0)
#define BLUE strip.Color(0,0,100)
#define YELLOW strip.Color(100,100,0)
#define OFF strip.Color(0,0,0)
#define BLANKS 39
#define SIDELEN 40

#define RINGLEN 16
#define STRIPLEN 160+RINGLEN

#define I2CADDR 0x4

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIPLEN, PIN, NEO_GRB + NEO_KHZ800);

uint8_t goti2c = 9;
uint8_t goti2carg = 0;
uint8_t pattern[16] = {0,0,0,0,0,20,60,100,150,255,255,255,150,100,60,20};
uint8_t place = 0;
uint8_t stopwait = 0;


#define NUM_LEDS          56
#define GRAVITY           -9.81              // Downward (negative) acceleration of gravity in m/s^2
#define h0                3                  // Starting height, in meters, of the ball (strip length)
#define NUM_BALLS         3                  // Number of bouncing balls you want (recommend < 7, but 20 is fun in its own way)

float h[NUM_BALLS] ;                         // An array of heights
float vImpact0 = sqrt( -2 * GRAVITY * h0 );  // Impact velocity of the ball when it hits the ground if "dropped" from the top of the strip
float vImpact[NUM_BALLS] ;                   // As time goes on the impact velocity will change, so make an array to store those values
float tCycle[NUM_BALLS] ;                    // The time since the last time the ball struck the ground
int   pos[NUM_BALLS] ;                       // The integer position of the dot on the strip (LED index)
long  tLast[NUM_BALLS] ;                     // The clock time of the last ground strike
float COR[NUM_BALLS] ;                       // Coefficient of Restitution (bounce damping)


void setup() {
  strip.begin();
  Wire.begin(I2CADDR);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  //Serial.begin(9600);           // start serial for output
  //Serial.print("setup\n\r");
  strip.show(); // Initialize all pixels to 'off'
  colorWipe(RED, 0);
  colorWipe(OFF,0);  
  
  for (int i = 0 ; i < NUM_BALLS ; i++) {    // Initialize variables
    tLast[i] = millis();
    h[i] = h0;
    pos[i] = 0;                              // Balls start on the ground
    vImpact[i] = vImpact0;                   // And "pop" up at vImpact0
    tCycle[i] = 0;
    COR[i] = 0.90 - float(i)/pow(NUM_BALLS,2); 
  }
}

void redfillpattern () {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(pattern[(i+place)%16],0,0));
  }
}

void yellowfillpattern () {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(pattern[(i+place)%16],pattern[(i+place)%16],0));
  }
}

void fillpatterncolor (uint8_t red, uint8_t green, uint8_t blue) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(pattern[(i+place)%16]*red, pattern[(i+place)%16]*green, pattern[(i+place)%16]*blue));
  }
}


void loop() {
  // Some example procedures showing how to display to the pixels:
  //colorWipe(strip.Color(255, 0, 0), 50); // Red
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  //colorWipe(strip.Color(0, 0, 255), 50); // Blue
  //rainbow(20);
  //rainbowCycle(20);
  stopwait = 0;
  if(goti2c == 1)   
    colorWipe(BLUE, 0);
  else if (goti2c == 2)
    colorFill(RED);
  else if (goti2c == 3)
    colorFill(GREEN);
  else if (goti2c == 4)
    colorFill(YELLOW);
  else if( goti2c == 0)
    doublebounce();
  else if( goti2c == 5)
    bloodmode();
  else if( goti2c == 6 )
    rainbow(0);
  else if( goti2c == 7)
    yellowbloodmode();
  else if( goti2c == 8)
    rainbowblink(30);
  else if( goti2c == 9)
    pulsemode(0,0,100);
}

uint8_t stopdelay(int16_t wait) {
  while(!stopwait && (wait -= 10) >= 0)
    delay( 10);
  return stopwait;
}   

void pulsemode(uint8_t red, uint8_t green, uint8_t blue) {
  for(uint8_t i=0; i<5; i++) {
      fillpatterncolor(red, green, blue);
      strip.show();
      place++;
      delay(10);
   }
   
   if(stopdelay(300)) return;

  for(uint8_t i=0; i<5; i++) {
      fillpatterncolor(red, green, blue);
      strip.show();
      place++;
      delay(10);
   }

   if(stopdelay(1000)) return;
}

void bloodmode() {
  for(uint8_t i=0; i<5; i++) {
      redfillpattern();
      strip.show();
      place++;
      delay(10);
   }
   
   if(stopdelay(300)) return;

  for(uint8_t i=0; i<5; i++) {
      redfillpattern();
      strip.show();
      place++;
      delay(10);
   }

   if(stopdelay(1000)) return;
}

void yellowbloodmode() {
  for(uint8_t i=0; i<5; i++) {
      yellowfillpattern();
      strip.show();
      place++;
      delay(10);
   }
   
   if(stopdelay(300)) return;

  for(uint8_t i=0; i<5; i++) {
      yellowfillpattern();
      strip.show();
      place++;
      delay(10);
   }

   if(stopdelay(1000)) return;
}


void receiveEvent(int howMany)
{
  char c = 0;
  while(1 < Wire.available()) // loop through all but the last
  {
    c = Wire.read(); // receive byte as a character
    Serial.println(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  //Serial.println(x);         // print the integer
  goti2c = c;
  goti2carg = x;
  stopwait = 1;
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void colorFill(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
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


void rainbowblink(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j+=3) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    if(stopdelay(wait)) return;
    for(i = 0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, OFF);
    }
    strip.show();
    if(stopdelay(wait)) return;
  }
}


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    if(stopdelay(wait)) return;
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
    if(stopdelay(wait)) return;
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

// from https://github.com/fibonacci162/LEDs/tree/master/BouncingBalls2014

void doublebounce() {
    for (int i = 0 ; i < NUM_BALLS ; i++) {
      tCycle[i] =  millis() - tLast[i] ;     // Calculate the time since the last time the ball was on the ground
  
      // A little kinematics equation calculates positon as a function of time, acceleration (gravity) and intial velocity
      h[i] = 0.5 * GRAVITY * pow( tCycle[i]/1000 , 2.0 ) + vImpact[i] * tCycle[i]/1000;
  
      if ( h[i] < 0 ) {                      
        h[i] = 0;                            // If the ball crossed the threshold of the "ground," put it back on the ground
        vImpact[i] = COR[i] * vImpact[i] ;   // and recalculate its new upward velocity as it's old velocity * COR
        tLast[i] = millis();
  
        if ( vImpact[i] < 0.01 ) vImpact[i] = vImpact0;  // If the ball is barely moving, "pop" it back up at vImpact0
      }
      pos[i] = round( h[i] * (NUM_LEDS - 1) / h0);       // Map "h" to a "pos" integer index position on the LED strip
    }
  
      strip.setPixelColor(pos[0] + RINGLEN + 24, RED);
      strip.setPixelColor(NUM_LEDS*2 - pos[0] + RINGLEN + 24, RED);
      strip.setPixelColor(pos[1] + RINGLEN + 24, GREEN);
      strip.setPixelColor(NUM_LEDS*2 - pos[1] + RINGLEN + 24,GREEN);
      strip.setPixelColor(pos[2] + RINGLEN + 24, BLUE);
      strip.setPixelColor(NUM_LEDS*2 - pos[2] + RINGLEN + 24, BLUE);

      strip.show();
      strip.setPixelColor(pos[0] + RINGLEN + 24, OFF);
      strip.setPixelColor(NUM_LEDS*2 - pos[0] + RINGLEN + 24, OFF);
      strip.setPixelColor(pos[1] + RINGLEN + 24, OFF);
      strip.setPixelColor(NUM_LEDS*2 - pos[1] + RINGLEN + 24,OFF);
      strip.setPixelColor(pos[2] + RINGLEN + 24, OFF);
      strip.setPixelColor(NUM_LEDS*2 - pos[2] + RINGLEN + 24, OFF);

      if(stopwait) return;
}

