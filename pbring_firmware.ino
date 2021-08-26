#include "usb.h"
#include "common.h"
#include "led.h"

void setup() {
	ledSetup();
//	usbSetup();
}

void loop() {
	currentMillis = millis();
//	usbLoop();
	ledLoop();
}
