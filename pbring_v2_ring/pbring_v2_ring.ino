#define FASTLED_INTERNAL

#include <FastLED.h>

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
	mCountdown
};

Mode mode;

/// In eights of a second
uint8_t countdownValue = 0;

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
}

void decideMode()
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
	else
	{
		mode = mCountdown;
		countdownValue = (4000 - (mls - 8000)) / (1000 / 8);
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
		animationIncrementInteval = lerp8by8(4, 16, min(255.0, constrain(countdownValue / 8.0 - 1.0, 0, 2) / 2.0 * 255.0));
		break;
	}
}

void loop()
{
	decideMode();
	setupAnimation();

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
			const uint8_t value = max(0, 255 - static_cast<int16_t>(diff) * 3);
			const uint8_t hue = useConstHue ? constHue : i * LED_INTERVAL + static_cast<uint8_t>(animationProgress);
			hsvData[i] = CHSV(hue, saturation, value);
		}
	}

	hsv2rgb_rainbow(hsvData, rgbData, LED_COUNT);
	FastLED.show();
}
