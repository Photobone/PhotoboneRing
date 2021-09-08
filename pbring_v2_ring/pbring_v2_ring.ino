#define FASTLED_INTERNAL

#include <FastLED.h>

#define TEST_MODE false

#define LED_COUNT 24
#define LED_PIN 2

#define LED_INTERVAL (256 / LED_COUNT)

CRGB rgbData[LED_COUNT];
CHSV hsvData[LED_COUNT];

enum Mode
{
	mNoInternalConnection,
	mNoConnection,
	mIdle,
	mPreview,
	mCountdown,
	mShooting
};

Mode mode;

bool signalReceived = false;
uint8_t countdownValue = 0; ///< In eights of a second

int16_t animationProgress = 0;

// Animation config
uint16_t animationIncrementInteval;
bool useConstHue;
uint8_t segmentCount;
uint8_t constHue;
uint8_t saturation;

void setup()
{
	FastLED.addLeds<NEOPIXEL, LED_PIN>(rgbData, LED_COUNT);

	// Only output is PB2
	DDRB = _BV(PB2);
}

void decideMode()
{
	if (TEST_MODE)
	{
		const unsigned long mls = millis() % 12000;
		if (mls < 2000)
		{
			mode = mNoInternalConnection;
		}
		else if (mls < 4000)
		{
			mode = mNoConnection;
		}
		else if (mls < 6000)
		{
			mode = mIdle;
		}
		else if (mls < 8000)
		{
			mode = mPreview;
		}
		else if (mls < 10000)
		{
			mode = mCountdown;
			countdownValue = (4000 - (mls - 8000)) / (1000 / 8);
		}
		else
		{
			mode = mShooting;
			countdownValue = 0;
		}
	}
	else
	{
		if (!signalReceived)
			mode = mNoInternalConnection;

		else if (countdownValue == -3)
			mode = mNoConnection;

		else if (countdownValue == -1)
			mode = mIdle;

		else if (countdownValue == -2)
			mode = mPreview;

		else if (countdownValue > 8)
			mode = mCountdown;

		else
			mode = mShooting;
	}
}

void setupAnimation()
{
	animationIncrementInteval = 16;
	useConstHue = false;
	segmentCount = 1;
	constHue = 0;
	saturation = 255;

	switch (mode)
	{

	case mNoInternalConnection:
		useConstHue = true;
		constHue = HUE_PURPLE;
		break;

	case mNoConnection:
		useConstHue = true;
		constHue = HUE_RED;
		break;

	case mIdle:
		useConstHue = true;
		saturation = 0;
		break;

	case mPreview:
		segmentCount = 2;
		break;

	case mCountdown:
		segmentCount = 3;
		animationIncrementInteval = lerp8by8(1, 16, min(255.0, constrain(countdownValue / 8.0 - 2.0, 0, 2) / 2.0 * 255.0));
		break;

	case mShooting:
		segmentCount = 0;
		animationIncrementInteval = 1;
		saturation = 0;
		break;
	}
}

uint8_t prevClockValue = 0;
uint8_t rcvLowData = 0;
void receiveData()
{
	const uint8_t clockValue = (PINB >> 1) & 1;

	if (clockValue == prevClockValue)
		return;

	prevClockValue = clockValue;

	// Clock is on low - just store the data and quit
	if (clockValue == LOW)
	{
		rcvLowData = PORTB & 1;
		return;
	}

	const uint8_t data = PORTB & 1;

	static uint8_t rcvBitCount = 0;
	static uint8_t receiveBuffer = 0;

	// Data on clock falling and raising edge differ - finished receiving
	if (data != rcvLowData)
	{
		rcvBitCount = 0;
		receiveBuffer = 0;
		return;
	}

	receiveBuffer = (receiveBuffer << 1) | data;
	if(++rcvBitCount == 8) {
		signalReceived = true;
		countdownValue = receiveBuffer;
	}
}

void loop()
{
	decideMode();
	setupAnimation();
	receiveData();

	// Update animation progress
	{
		const unsigned long currentMillis = millis();
		static unsigned long prevMillis = currentMillis;
		static uint16_t millisAccum = 0;

		millisAccum += static_cast<uint16_t>(currentMillis - prevMillis);
		prevMillis = currentMillis;

		while (millisAccum >= animationIncrementInteval)
		{
			animationProgress++;
			millisAccum -= animationIncrementInteval;
		}
	}

	// Animate
	{
		for (uint8_t i = 0; i < LED_COUNT; i++)
		{
			const uint8_t diff = abs(static_cast<int8_t>(i * LED_INTERVAL * segmentCount - static_cast<uint8_t>(animationProgress)));
			const uint8_t value = max(0, 255 - static_cast<int16_t>(diff) * (segmentCount == 1 ? 5 : 3));
			const uint8_t hue = useConstHue ? constHue : i * LED_INTERVAL + static_cast<uint8_t>(animationProgress);
			hsvData[i] = CHSV(hue, saturation, value);
		}
	}

	hsv2rgb_rainbow(hsvData, rgbData, LED_COUNT);
	FastLED.show();
}
