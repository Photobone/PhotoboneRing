# Photobone Ring
LED Ring project to be used together with Photobone Studio.

The firmware is using the [V-USB](https://www.obdev.at/products/vusb/index.html) library under the GPL v2 license. This project itself is also published under the GPL v2 license.

## Required components
* 2 ATTINY85 arduinos: one controlling ring, one communicating through USB with PC
	* The arduinos are connected as described in the Pinout section

* WS2812 (not WS2812B - you might need to change some code for that) 24-LED ring (different LED count theoretically possible when adjusting source code)

## Status indication
* Violet: booting/waiting for arduinos to communicate
* Red: not connected with Photobone Studio

# Pinout
* RING - Arduino controlling the ring
* USB - Arduino controlling the usb
* LED - The LED ring itself

#### RING Digispark/Attiny85:
* RING P0 - USB P0
* RING P1 - LED DI
* RING P2 - USB P1

* RING 5V - USB 5V
* RING GND - USB GND

#### USB Digispark/Attiny85:
* USB P0 - RING P0
* USB P1 - RING P2
* USB P4 - USB +
* USB P5 - USB -

* RING 5V - USB 5V
* RING GND - USB GND