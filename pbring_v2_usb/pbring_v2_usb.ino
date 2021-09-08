#include <Arduino.h>
#include <util/delay.h>

extern "C"
{
#include "usbdrv.h"
}

bool isConnected = false;
uint8_t countdownValue = 0;
unsigned long currentMillis = 0, heartbeatTimeoutMillis = 0, nextTransmitMillis = 0;

void transmitData()
{
	if (currentMillis < nextTransmitMillis)
		return;

	nextTransmitMillis = currentMillis + 100;

	uint8_t data = isConnected ? countdownValue : -3;
	for (uint8_t i = 0; i < 8; i++)
	{
		delayMicroseconds(10);

		// Clock low
		PORTB &= ~_BV(PB1);

		delayMicroseconds(10);

		// Set the data output
		PORTB = (PORTB & ~1) | (data & 1);

		delayMicroseconds(10);

		// Clock high
		PORTB |= _BV(PB1);

		delayMicroseconds(10);

		// Shift data
		data >>= 1;
	}
}

void setup()
{
	// Pins 0 and 1 set as outputs
	DDRB = _BV(PB0) | _BV(PB1);

	// Setup USB
	{
		cli();

		usbInit();
		usbDeviceDisconnect();

		// fake USB disconnect for > 250 ms - required in example?
		uchar i = 0;
		while (--i)
			_delay_ms(10);

		usbDeviceConnect();

		sei();
	}
}

void loop()
{
	currentMillis = millis();

	usbPoll();

	transmitData();

	isConnected = currentMillis < heartbeatTimeoutMillis;
}

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	usbRequest_t *rq = reinterpret_cast<usbRequest_t *>(data);

	if ((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_CLASS)
		return 0;

	if (rq->bRequest == USBRQ_HID_SET_REPORT)
	{
		return USB_NO_MSG;
	}

	return 0;
}

uint8_t usbFunctionWrite(uint8_t *data, uint8_t len)
{
	/*if (len == 0 || !isExpectingMessage)
		return !isExpectingMessage;*/

	countdownValue = *data;
	heartbeatTimeoutMillis = currentMillis + 5000;

	// We've received all data, thanks
	return 1;
}

PROGMEM const char usbHidReportDescriptor[22] = {
		0x06, 0x00, 0xff, // USAGE_PAGE (Generic Desktop)
		0x09, 0x01,				// USAGE (Vendor Usage 1)
		0xa1, 0x01,				// COLLECTION (Application)
		0x15, 0x00,				//   LOGICAL_MINIMUM (0)
		0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
		0x75, 0x08,				//   REPORT_SIZE (8)
		0x95, 0x01,				//   REPORT_COUNT (1)
		0x09, 0x00,				//   USAGE (Undefined)
		0xb2, 0x02, 0x01, //   FEATURE (Data,Var,Abs,Buf)
		0xc0							// END_COLLECTION
};