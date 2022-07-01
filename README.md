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
* ARD_RING - Arduino controlling the ring
* ARD_USB - Arduino controlling the usb
* USB_PORT - connection with the USB port (no need to wire anything, is done on the board itself)
* LED_RING - The LED ring itself

#### ARD_RING Digispark/Attiny85:
* ARD_RING P0 - ARD_USB P0
* ARD_RING P1 - LED_RINGDI
* ARD_RING P2 - ARD_USB P1

* ARD_RING 5V - ARD_USB 5V - LED +
* ARD_RING GND - ARD_USB GND - LED -

#### ARD_USB Digispark/Attiny85:
* ARD_USB P0 - ARD_RING P0
* ARD_USB P1 - ARD_RING P2
* ARD_USB P4 - USB_PORT +
* ARD_USB P5 - USB_PORT -