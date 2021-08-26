#include "usb.h"

#include <Arduino.h>
#include <util/delay.h>

extern "C"
{
#include "usbdrv.h"
}

#include "common.h"

bool isConnected = false;
uint8_t countdownValue = -1;

static unsigned long lastHeartbeatMillis = 0;

void usbSetup()
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

void usbLoop()
{
	usbPoll();

	isConnected = currentMillis <= lastHeartbeatMillis + 10000;
}

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	usbRequest_t *rq = reinterpret_cast<usbRequest_t *>(data);

	if ((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_CLASS)
		return 0;

	if (rq->bRequest == USBRQ_HID_SET_REPORT)
		return USB_NO_MSG;

	return 0;
}

uint8_t usbFunctionWrite(uint8_t *data, uint8_t len)
{
	countdownValue = *data;
	lastHeartbeatMillis = currentMillis;

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