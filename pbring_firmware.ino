#include "led_driver.h"


void setup() {
  ledSetup();
}

long startTime = 0;
void loop() {
  if (millis() < 1500) {
    ledLoop(POWER_UP, millis());
  } else if(millis() < 10000) {
	  ledLoop(PHOTO_IDLE, millis());
  } else {
    if (startTime == 0) startTime = millis();
    ledLoop(PHOTO_TIMER, millis() - startTime);
  }
}
