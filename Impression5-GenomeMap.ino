/**
  \file Impression5-GenomeMap.ino
  \brief Main Arduino sketch for Impression 5s Genome Migration Map Interactive Game.
  \remarks comments are implemented with Doxygen Markdown format
  \author  Michael Flaga, michael@.flaga.net
*/

#include "MPR121.h"
#include <Wire.h>
#define LENGTH_OF_ARRAY(x) ((sizeof(x)/sizeof(x[0])))
#define numElectrodes 12 // per MPR chip

typedef struct 
  {
      MPR121_t device;
      uint8_t address;
      unsigned char tthresh[numElectrodes];
      unsigned char rthresh[numElectrodes];
      uint8_t identifier[numElectrodes];
  } mprs;

MPR121_t MPR121A;
MPR121_t MPR121B; // additional MPR chips will be defined but not all may be used.
MPR121_t MPR121C; // 4 maximimum possible on single i2c bus
MPR121_t MPR121D;

mprs chips[] = {
                    (mprs) {MPR121A, 0x5A,                           
                      {10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10}, //tthresh      
                      {05,   05,   05,   05,   05,   05,   05,   05,   05,   05,   05,   05}, //rthresh        
                      { 1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12}, //identifier     
                    }/*, // move start of comment to define number of MPR chips.
                    
                    (mprs) {MPR121B, 0x5B, 
                      {20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20}, //tthresh      
                      {10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10}, //rthresh     
                      {13,   14,   15,   16,   17,   18,   19,   20,   21,   22,   23,   24}, //identifier     
                    },
                    (mprs) {MPR121C, 0x5C, 
                      {20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20}, //tthresh      
                      {10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10}, //rthresh     
                      {25,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36}, //identifier     
                    },
                    (mprs) {MPR121D, 0x5D, 
                      {20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20}, //tthresh      
                      {10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10}, //rthresh     
                      {37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,   48}, //identifier     
                    }*/
                  };

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

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 1000/4;           // interval at which to blink (milliseconds)
int strandsNumberOfLEDs;


void updateSegment(int startLEDpos, int endLEDpos, int r, int g, int b ) {
  for (int pixel = startLEDpos ; pixel <= endLEDpos; pixel++) {
    strip.setPixelColor(pixel, strip.Color(r, g, b));
  }
}

void setup()
{

  Serial.begin(115200);
  //while(!Serial);  // only needed if you want serial feedback with the
  		   // Arduino Leonardo or Bare Touch Board
  Serial.println("Setup() starting ");
  Wire.begin();
  
  for(int index=0; index<LENGTH_OF_ARRAY(chips); index++){
    Serial.print("Initializing MPR #");
    Serial.print(index);
    Serial.println();

    if(!chips[index].device.begin(chips[index].address)){ 
      Serial.println("error setting up MPR121");  
      switch(chips[index].device.getError()){
        case NO_ERROR:
          Serial.println("no error");
          break;  
        case ADDRESS_UNKNOWN:
          Serial.println("incorrect address");
          break;
        case READBACK_FAIL:
          Serial.println("readback failure");
          break;
        case OVERCURRENT_FLAG:
          Serial.println("overcurrent on REXT pin");
          break;      
        case OUT_OF_RANGE:
          Serial.println("electrode out of range");
          break;
        case NOT_INITED:
          Serial.println("not initialised");
          break;
        default:
          Serial.println("unknown error");
          break;      
      }
      while(1);
    }

    // pin 4 is the MPR121 interrupt on the Bare Touch Board
    chips[index].device.setInterruptPin(7);

    for(unsigned char channel=0; channel < numElectrodes; channel++){

      // this is the touch threshold - setting it low makes it more like a proximity trigger
      // default value is 40 for touch
      chips[index].device.setTouchThreshold(channel, chips[index].tthresh[channel]);
      
      // this is the release threshold - must ALWAYS be smaller than the touch threshold
      // default value is 20 for touch
      chips[index].device.setReleaseThreshold(channel, chips[index].rthresh[channel]);  
    }
    
    // initial data update
    chips[index].device.updateTouchData();
  }


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
  
  updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 122,40,5 );
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250);
  channel++;

  updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 244,11,146 );
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250);
  channel++;

  updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 181,74,143 );
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250);
  channel++;
    
  updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 255,0,0 );
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250);
  channel++;

  updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 0,0,255 );
  strip.show(); // This sends the updated pixel color to the hardware.
  delay(250);
  channel++;
  
  Serial.println("Setup() done ");
}

void loop()
{
  unsigned long currentMillis = millis();

	// Scan capacitive touch sensors for new input.
  for(int index=0; index<LENGTH_OF_ARRAY(chips); index++){
    if(chips[index].device.touchStatusChanged()){
      chips[index].device.updateTouchData();
      for(uint8_t electrode=0; electrode < numElectrodes; electrode++){
          if(chips[index].device.isNewTouch(electrode)){
            Serial.print("device ");
            Serial.print(index, DEC);
            Serial.print(" electrode ");
            Serial.print(electrode, DEC);
            Serial.print(" Identifier 0x");
            Serial.print(chips[index].identifier[electrode], DEC);
            Serial.println(" was just touched!");  
            if (chips[index].identifier[electrode] == 12) {
              updateSegment(0, NUMPIXELS - 1, 0,0,0 );
              updateSegment(segment[0].startLEDpos, segment[0].endLEDpos, 0,255,0 );
              strip.show(); // This sends the updated pixel color to the hardware.              
            }
            
          } else if(chips[index].device.isNewRelease(electrode)){
            Serial.print("device ");
            Serial.print(index, DEC);
            Serial.print(" electrode ");
            Serial.print(electrode, DEC);
            Serial.println(" was just released.");  
            if (chips[index].identifier[electrode] == 12) {
              int channel = 0;
              updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 122,40,5 );
              channel++;
              updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 244,11,146 ); 
              channel++;
              updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 181,74,143 ); 
              channel++;
              updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 255,0,0 );
              channel++;
              updateSegment(segment[channel].startLEDpos, segment[channel].endLEDpos, 0,0,255 );
              strip.show(); // This sends the updated pixel color to the hardware.              
            }
          }
      } 
    }
  }

}
