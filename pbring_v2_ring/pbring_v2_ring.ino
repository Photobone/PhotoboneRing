#define FASTLED_INTERNAL

#include <FastLED.h>

#define LED_COUNT 24
#define LED_PIN 2

#define LED_INTERVAL (256 / LED_COUNT)

CRGB rgbData[LED_COUNT];
CHSV hsvData[LED_COUNT];

uint16_t animationProgress = 0;

void setup()
{
	FastLED.addLeds<NEOPIXEL, LED_PIN>(rgbData, LED_COUNT);
}

void loop()
{
	// Increment animation progress
	{
		const unsigned long currentMillis = millis();
		static unsigned long prevMillis = currentMillis;
		static uint16_t millisAccum = 0;

		millisAccum += static_cast<uint16_t>(currentMillis - prevMillis);
		prevMillis = currentMillis;

		const uint16_t animIncInterval = 16;
		while (millisAccum >= animIncInterval)
		{
			animationProgress++;
			millisAccum -= animIncInterval;
		}
	}

	for (uint8_t i = 0; i < LED_COUNT; i++)
	{
		//hsvData[i] = CHSV(i * LED_INTERVAL /*+ static_cast<uint8_t>(animationProgress)*/, 255, 255 - constrain(abs8(static_cast<int8_t>(i * LED_INTERVAL) - static_cast<int8_t>(animationProgress / 2)), 0,  * 4);
		const uint8_t value = 255 - min(abs8(i * LED_INTERVAL - animationProgress / 2), 127) * 2;
		hsvData[i] = CHSV(i * LED_INTERVAL, 255, value);
	}

	hsv2rgb_rainbow(hsvData, rgbData, LED_COUNT);
	FastLED.show();
}
