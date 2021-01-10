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

const long interval = 50;
unsigned long previousMillis = 0;
int8_t lastxAxis = 0;
int8_t lastyAxis = 0;
int8_t lasttAxis = 0;
int8_t lastAxis[] = {0,0,0};


void setup()
{

  Serial.begin(115200);
  //while(!Serial);  // only needed if you want serial feedback with the
  		   // Arduino Leonardo or Bare Touch Board
  Serial.println("started");
  Wire.begin();
  
  // 0x5C is the MPR121 I2C address on the Bare Touch Board
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

  Serial.println("end setup");
}

void loop()
{
  unsigned long currentMillis = millis();

	// Scan capacitive touch sensors for new input.
  for(int index=0; index<LENGTH_OF_ARRAY(chips); index++){
    if(chips[index].device.touchStatusChanged()){
      chips[index].device.updateTouchData();
      for(uint8_t channel=0; channel < numElectrodes; channel++){
          if(chips[index].device.isNewTouch(channel)){
            Serial.print("device ");
            Serial.print(index, DEC);
            Serial.print(" electrode ");
            Serial.print(channel, DEC);
            Serial.print(" Identifier 0x");
            Serial.print(chips[index].identifier[channel], HEX);
            Serial.println(" was just touched!");  
          } else if(chips[index].device.isNewRelease(channel)){
            Serial.print("device ");
            Serial.print(index, DEC);
            Serial.print(" electrode ");
            Serial.print(channel, DEC);
            Serial.println(" was just released.");  
          }
      } 
    }
  }

}
