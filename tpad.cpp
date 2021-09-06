#include "Arduino.h"
#include "tpad.h"
#include <avr/pgmspace.h>

tpad::tpad() {
}

tpad::~tpad() {
}

void tpad::begin(int IRQpin) {
  interruptPin = IRQpin;
  
  for (int index = 0; index < LENGTH_OF_ARRAY(chips); index++) {
    Serial.print("Initializing MPR #");
    Serial.print(index);
    Serial.println();

    if (!chips[index].device.begin(chips[index].address)) {
      Serial.println("error setting up MPR121");
      switch (chips[index].device.getError()) {
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
      while (1);
    }

    // pin 4 is the MPR121 interrupt on the Bare Touch Board
    chips[index].device.setInterruptPin(interruptPin);

    for (unsigned char channel = 0; channel < numElectrodes; channel++) {

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

}

int tpad::scan(int targetButton) {
  // Scan capacitive touch sensors for new input.

  if (::digitalRead(interruptPin)) {
    return 0;
  }

  
  for (int index = 0; index < LENGTH_OF_ARRAY(chips); index++) {
    if (chips[index].device.touchStatusChanged()) {
      chips[index].device.updateTouchData();
      for (uint8_t electrode = 0; electrode < numElectrodes; electrode++) {
        if (chips[index].device.isNewTouch(electrode)) {
          Serial.print("device ");
          Serial.print(index, DEC);
          Serial.print(" electrode ");
          Serial.print(electrode, DEC);
          Serial.print(" Identifier 0x");
          Serial.print(chips[index].identifier[electrode], DEC);
          Serial.println(" was just touched!");
          if (chips[index].identifier[electrode] == targetButton) {
            return +targetButton;
          }

        } else if (chips[index].device.isNewRelease(electrode)) {
          Serial.print("device ");
          Serial.print(index, DEC);
          Serial.print(" electrode ");
          Serial.print(electrode, DEC);
          Serial.println(" was just released.");
          if (chips[index].identifier[electrode] == targetButton) {
            return -targetButton;
          }
        }
      }
    }
  }
  return 0;
}
