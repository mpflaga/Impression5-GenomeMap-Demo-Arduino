/**
  \file Impression5-GenomeMap.ino
  \brief Main Arduino sketch for Impression 5s Genome Migration Map Interactive Game.
  \remarks comments are implemented with Doxygen Markdown format
  \author  Michael Flaga, michael@.flaga.net
*/

#include "tpad.h"
#include <Wire.h>
#define LENGTH_OF_ARRAY(x) ((sizeof(x)/sizeof(x[0])))



#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 32
#define NEOPIXLPIN 6
#define LENGTH_OF_ARRAY(x) ((sizeof(x)/sizeof(x[0])))

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, NEOPIXLPIN, NEO_GRB + NEO_KHZ800);

class segments
{
  public:
    int section;
    int numberOfLEDs;
    int startLEDpos;
    int endLEDpos;
    unsigned long interval;
    unsigned long previousMillis;
    int r[2], g[2], b[2];

} segment[17];

tpad tpad;

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 1000 / 4;         // interval at which to blink (milliseconds)
int strandsNumberOfLEDs;


void updateSegment(int startLEDpos, int endLEDpos, int r, int g, int b ) {
  for (int pixel = startLEDpos ; pixel <= endLEDpos; pixel++) {
    strip.setPixelColor(pixel, strip.Color(r, g, b));
  }
}

void setup()
{

  Serial.begin(115200);
  //while (!Serial); // only needed if you want serial feedback with the
  // Arduino Leonardo or Bare Touch Board
  Serial.println("Setup() starting Version 2");
  Wire.begin();

  tpad.begin();

  // Too many Segments and NeoPixel for UNOs 2K of RAM, need to use Mega with 8K of RAM.
  segment[0].numberOfLEDs  = 16;
  segment[0].section  =  0;
  segment[1].numberOfLEDs  = 4;
  segment[1].section  =  1;
  segment[2].numberOfLEDs  = 4;
  segment[2].section  =  2;
  segment[3].numberOfLEDs  = 4;
  segment[3].section  =  3;
  segment[4].numberOfLEDs  = 4;
  segment[4].section  =  4;

  strandsNumberOfLEDs = 0;

  for (int i = 0; i < LENGTH_OF_ARRAY(segment); i++) {
    segment[i].interval = 0;
    segment[i].r[0] = 0;
    segment[i].g[0] = 0;
    segment[i].b[0] = 0;
    segment[i].r[1] = 0;
    segment[i].g[1] = 0;
    segment[i].b[1] = 0;
    segment[i].startLEDpos = strandsNumberOfLEDs;
    strandsNumberOfLEDs += segment[i].numberOfLEDs - 1;
    segment[i].endLEDpos = strandsNumberOfLEDs;
    strandsNumberOfLEDs++;
    Serial.print("segment["); Serial.print(i); Serial.print("].section = "); Serial.print(segment[i].section);
    Serial.print(", segment["); Serial.print(i); Serial.print("].startLEDpos = "); Serial.print(segment[i].startLEDpos);
    Serial.print(", segment["); Serial.print(i); Serial.print("].endLEDpos = "); Serial.println(segment[i].endLEDpos);
  }

  Serial.println("Setup() NeoPixels ");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Serial.println("Setup() NeoPixels, all RED");
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
  }
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250); // Delay for a period of time (in milliseconds).

  Serial.println("Setup() NeoPixels, all GRN");
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0));
  }
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250); // Delay for a period of time (in milliseconds).

  Serial.println("Setup() NeoPixels, all BLUE");
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 255));
  }
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250); // Delay for a period of time (in milliseconds).

  Serial.println("Setup() NeoPixels, all OFF");
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show(); // This sends the updated pixel color to the hardware.



  int channel = 0;

  updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 122, 40, 5 );
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250);
  channel++;

  updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 244, 11, 146 );
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250);
  channel++;

  updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 181, 74, 143 );
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250);
  channel++;

  updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 255, 0, 0 );
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250);
  channel++;

  updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 0, 0, 255 );
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250);
  channel++;

  Serial.println("Setup() done ");
}

void loop()
{
  unsigned long currentMillis = millis();

  int temp = tpad.scan(12);
  if (temp > 0) {
    Serial.println("12 was touched");
    updateSegment(0, NUMPIXELS - 1, 0, 0, 0 );
    updateSegment(segment[0].startLEDpos, segment[0].endLEDpos, 0, 255, 0 );
    strip.show(); // This sends the updated pixel color to the hardware.
  } else if (0 > temp) {
    Serial.println("12 was released");
    int channel = 0;
    updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 122, 40, 5 );
    channel++;
    updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 244, 11, 146 );
    channel++;
    updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 181, 74, 143 );
    channel++;
    updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 255, 0, 0 );
    channel++;
    updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 0, 0, 255 );
    strip.show(); // This sends the updated pixel color to the hardware.         }
  }
}
