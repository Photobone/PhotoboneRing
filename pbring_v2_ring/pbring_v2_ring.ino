#define FASTLED_INTERNAL

#include <FastLED.h>

#define LED_COUNT 24
#define LED_PIN 1

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

unsigned long currentMillis = 0, nextDataReceiveMillis = 0;

// Animation config
uint16_t animationIncrementInteval;
bool useConstHue;
uint8_t segmentCount;
uint8_t constHue;
uint8_t saturation;

float mapf(float v, float srcL, float srcH, float tgtL, float tgtH)
{
	return tgtL + constrain((v - srcL) / (srcH - srcL), 0.0, 1.0) * (tgtH - tgtL);
}

void setup()
{
	FastLED.addLeds<NEOPIXEL, LED_PIN>(rgbData, LED_COUNT);

	// Only output is PB1 (leds)
	DDRB = _BV(PB1);

	// No pull down all pins
	PORTB = 0;

	// USI control register - two wire mode, external clock rising edge
	USICR = _BV(USIWM1) | _BV(USICS1);

	// Reset USI status
	USISR = _BV(USIOIF);
}

void decideMode()
{
	if (!signalReceived)
		mode = mNoInternalConnection;

	else if (countdownValue == static_cast<uint8_t>(-3))
		mode = mNoConnection;

	else if (countdownValue == static_cast<uint8_t>(-1))
		mode = mIdle;

	else if (countdownValue == static_cast<uint8_t>(-2))
		mode = mPreview;

	else if (countdownValue > 8)
		mode = mCountdown;

	else
		mode = mShooting;
}

void setupAnimation()
{
	animationIncrementInteval = 16;
	useConstHue = false;
	segmentCount = 3;
	constHue = 0;
	saturation = 255;

	switch (mode)
	{

	case mNoInternalConnection:
		useConstHue = true;
		constHue = HUE_PURPLE;
		segmentCount = 1;
		break;

	case mNoConnection:
		useConstHue = true;
		constHue = HUE_RED;
		segmentCount = 1;
		break;

	case mIdle:
		break;

	case mPreview:
		saturation = 0;
		break;

	case mCountdown:
		animationIncrementInteval = 1.0 / mapf(countdownValue / 8.0, 1, 4, 1.0 / 1, 1.0 / 8);
		saturation = 0;
		break;

	case mShooting:
		segmentCount = 0;
		animationIncrementInteval = 1;
		saturation = 0;
		break;
	}
}

void receiveData()
{
	if (currentMillis < nextDataReceiveMillis)
		return;

	nextDataReceiveMillis = currentMillis + 16;

	// Check counter overflow (means we have received the message)
	if (USISR & _BV(USIOIF))
	{
		// Ignore first few reports
		static uint8_t rcvCnt = 0;
		if (rcvCnt < 8)
			rcvCnt++;
		else
			signalReceived = true;

		countdownValue = USIBR;
		USISR = _BV(USIOIF);
	}
}

void loop()
{
	currentMillis = millis();
	uint8_t delta = 0;

	decideMode();
	setupAnimation();
	receiveData();

	// Update animation progress
	{
		static unsigned long prevMillis = currentMillis;
		static uint16_t millisAccum = 0;

		millisAccum += static_cast<uint16_t>(currentMillis - prevMillis);
		prevMillis = currentMillis;

		while (millisAccum >= animationIncrementInteval)
		{
			animationProgress++;
			delta++;
			millisAccum -= animationIncrementInteval;
		}
	}

	// Animate
	{
		static uint8_t actualSaturation = saturation;
		if (saturation < actualSaturation)
			actualSaturation -= min(actualSaturation - saturation, delta * 4);
		else if (saturation > actualSaturation)
			actualSaturation += min(saturation - actualSaturation, delta * 4);

		for (uint8_t i = 0; i < LED_COUNT; i++)
		{
			const uint8_t diff = abs(static_cast<int8_t>(i * LED_INTERVAL * segmentCount - static_cast<uint8_t>(animationProgress)));
			const uint8_t value = max(0, 255 - static_cast<int16_t>(diff) * (segmentCount == 1 ? 5 : 3));
			const uint8_t hue = useConstHue ? constHue : i * LED_INTERVAL + static_cast<uint8_t>(animationProgress);
			hsvData[i] = CHSV(hue, actualSaturation, value);
		}
	}

	hsv2rgb_rainbow(hsvData, rgbData, LED_COUNT);

	// This just shows the countdown value in binary on the leds
	if (0)
	{
		for (uint8_t i = 0; i < 8; i++)
			rgbData[i] = (countdownValue & (1 << i)) ? 0xffffff : 0x000000;
	}

	FastLED.show();
}
