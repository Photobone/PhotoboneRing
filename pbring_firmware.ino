#include "usb.h"
#include "common.h"
#include "led.h"

void setup() {
	// Init leds first so that the USB has no interruptions after setup
	ledSetup();

	//usbSetup();
}

void loop() {
	currentMillis = millis();

	//usbLoop();
	ledLoop();
}
