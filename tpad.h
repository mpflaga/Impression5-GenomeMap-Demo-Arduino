#ifndef IFDEBUG
#define IFDEBUG(...) ((void)((DEBUG_LEVEL) && (__VA_ARGS__, 0)))
#define DEBUG_LEVEL 0 // set to 1 to compile in Serial Debug prints
#endif

#ifndef LENGTH_OF_ARRAY
#define LENGTH_OF_ARRAY(x) ((sizeof(x)/sizeof(x[0])))
#endif

#ifndef tpad_H
#define tpad_H

#include "MPR121.h"

#define numElectrodes 12 // per MPR chip


typedef struct {
  MPR121_t device;
  uint8_t address;
  unsigned char tthresh[numElectrodes];
  unsigned char rthresh[numElectrodes];
  uint8_t identifier[numElectrodes];
} mprs;

class tpad {

  public:
    tpad();
    ~tpad();
    void begin(int);
    int scan(int);

  private:
    int interruptPin;
    MPR121_t MPR121A;
#if defined(__AVR_ATmega2560__)
    MPR121_t MPR121B; // additional MPR chips will be defined but not all may be used.
    MPR121_t MPR121C; // 4 maximimum possible on single i2c bus
    MPR121_t MPR121D;
#endif

#if defined(__AVR_ATmega32U4__)
    // Demo Board with just one Touch Chip
    mprs chips[1] = {
      (mprs) {
        MPR121A, 0x5A,
          {10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10}, //tthresh
          {05,   05,   05,   05,   05,   05,   05,   05,   05,   05,   05,   05}, //rthresh
          {01,   02,   03,   04,   05,   06,   07,   08,   09,   10,   11,   12}, //identifier
      } // move start of comment to define number of MPR chips.
    };
#elif defined(__AVR_ATmega2560__)
    // Fully System with four chips
    mprs chips[4] = {
      (mprs) {
        MPR121A, 0x5A,
          {10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10}, //tthresh
          {05,   05,   05,   05,   05,   05,   05,   05,   05,   05,   05,   05}, //rthresh
          { 4,   26,    0,    0,    0,   16,   18,    3,   19,   17,   12,   24}, //identifier
/*         AUS,  SE_ASI,na,   na,   na,  India,Kazak,Asia, Korea,Japa, China,PacIsl*/        
        },
        {MPR121B, 0x5B,
          {20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20}, //tthresh
          {10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10}, //rthresh
          {13,   14,   15,   16,   17,   18,   19,   20,   21,   22,   23,   24}, //identifier
        },
        {MPR121C, 0x5C,
          {20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20}, //tthresh
          {10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10}, //rthresh
          {25,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36}, //identifier
        },
        {MPR121D, 0x5D,
          {20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20,   20}, //tthresh
          {10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10}, //rthresh
          {37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,   48}, //identifier
        }/* */
    };
#endif
};

#endif // tpad_H
