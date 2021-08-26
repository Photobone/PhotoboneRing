#include "usb.h"
#include "common.h"

void setup() {
	usbSetup();
}

void loop() {
	currentMillis = millis();
	
	usbLoop();
}
