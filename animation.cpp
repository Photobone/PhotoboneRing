#include "animation.h"
#include "led.h"
#include "common.h"
#include <Arduino.h>

float GREEN[] = {0.0f, 1.0f, 0.0f};
float RED[] = {1.0f, 0.0f, 0.0f};
float WHITE[] = {1.0f, 1.0f, 1.0f};
float BLACK[] = {0.0f, 0.0f, 0.0f};

/*private*/ void systemFlash(uint16_t progress, const float *rgb)
{
  
	const int UP_DELAY_MS = 13;
	const int ALL_ON_MS = 100;
	const int TWI_MS = 150;

	const uint16_t twiStart = UP_DELAY_MS * NUMPIXELS + ALL_ON_MS;

	const bool cond = progress < twiStart;

	const float factor = cond ? 1 : max(0, (float)(TWI_MS - (progress - twiStart)) / TWI_MS);
	currentRgb[0] = rgb[0] * factor;
	currentRgb[1] = rgb[1] * factor;
	currentRgb[2] = rgb[2] * factor;

  uint8_t i = 0;
	if (cond)
	{
		for (; progress / UP_DELAY_MS > i; i++)
		{
			storeCurrentRgb(i);
		}

		currentRgb[0] = 0.0f;
		currentRgb[1] = 0.0f;
		currentRgb[2] = 0.0f;

	}
		
	for (; i < NUMPIXELS; i++)
	{
		storeCurrentRgb(i);
	}
	
}


#define IDLE_RAINBOW_SPEED 0.0001f
#define RAINBOW_ALPHA_FACTOR 3

float rainbowOffset = 0;
/*private*/ void rainbowUpdate(uint16_t deltaMs, float speed, bool enableAlpha, float brightness)
{

  rainbowOffset += static_cast<float>(deltaMs) * speed;
  rainbowOffset = fmod(rainbowOffset, 1);

	for (uint8_t i = 0; i < NUMPIXELS; i++)
	{

		float h = static_cast<float>(i) / NUMPIXELS + rainbowOffset;

		float alpha = enableAlpha ? max(0, abs(fmod(static_cast<float>(i) / NUMPIXELS + rainbowOffset * RAINBOW_ALPHA_FACTOR, 1.0f) - 0.5f) * 5 - 1.5f) : 1;

		hsv2Rgb(h, 1.0f, 0.05f * alpha * brightness, currentRgb);
		currentRgb[0] *= 3;
		currentRgb[1] *= 1;
		currentRgb[2] *= 1.35f;

		storeCurrentRgb(i);
	}
}


/*private*/ void photoTimerLoop(uint32_t progress, uint16_t deltaMs)
{
	const float ROTATION_SPEED_START = 0.00015f;
	const float ROTATION_SPEED_END = 0.006f;

	const int ROTATION_SPEEDUP_DURATION_MS = 3500;

	const int BLINK_DURATION_MS = 3000;
	const int BLINK_INTERVAL_MS = 150;

	if (progress < ROTATION_SPEEDUP_DURATION_MS)
	{

    float speedup = static_cast<float>(progress) / ROTATION_SPEEDUP_DURATION_MS;
    rainbowUpdate(deltaMs, speedup * (ROTATION_SPEED_END - ROTATION_SPEED_START) + ROTATION_SPEED_START, 0, speedup + 1);
  
	}
	else
	{
		const bool cond =
				(progress < BLINK_DURATION_MS + ROTATION_SPEEDUP_DURATION_MS) &&
				((progress - ROTATION_SPEEDUP_DURATION_MS) / BLINK_INTERVAL_MS) % 2;

		currentRgb[0] = cond ? WHITE[0] * 0.6f : BLACK[0];
		currentRgb[1] = cond ? WHITE[1] * 0.6f : BLACK[1];
		currentRgb[2] = cond ? WHITE[2] * 0.6f : BLACK[2];

		for (uint8_t i = 0; i < NUMPIXELS; i++)
		{
			storeCurrentRgb(i);
		}
	}
}

inline ANIMATION resolveAnimation()
{
  if (currentMillis < 1500) {
    return POWER_UP;
  }
  
  long period = currentMillis % 15000;
  if (period < 11000) {
    return PHOTO_TIMER;  
  } else {
    return PHOTO_IDLE;
  }

	
//	
//	if (currentMillis < 1500)
//	{
//		return POWER_UP;
//	}
//	else if (currentMillis < 10000)
//	{
//		return PHOTO_IDLE;
//	}
//	else if (currentMillis < 20000)
//	{
//		return PHOTO_TIMER;
//	}
//	else
//	{
//		return PHOTO_IDLE;
//	}
}

ANIMATION lastAnimation = NULL_BLACK;
long lastAnimationStart = 0;
long lastFrameTime = 0;
void updateAnimation()
{
	ANIMATION currentAnim = resolveAnimation();

	if (lastAnimation != currentAnim)
	{
		lastAnimation = currentAnim;
		lastAnimationStart = currentMillis;
	}
	long sinceStart = currentMillis - lastAnimationStart;
  uint16_t deltaMs = currentMillis - lastFrameTime;
  lastFrameTime = currentMillis;

	switch (currentAnim)
	{

	case POWER_UP:
	case COMM_ERR:
		systemFlash(sinceStart, currentAnim == POWER_UP ? GREEN : RED);
		break;

	case PHOTO_IDLE:
		rainbowUpdate(deltaMs, IDLE_RAINBOW_SPEED, 1, 1.0f);
		break;

	case PHOTO_TIMER:
		photoTimerLoop(sinceStart, deltaMs);
		break;

		/*case NULL_BLACK:
		currentRgb[0] = BLACK[0];
		currentRgb[1] = BLACK[1];
		currentRgb[2] = BLACK[2];

		for (uint8_t i = 0; i < NUMPIXELS; i++)
		{
			storeCurrentRgb(i);
		}
		break;*/
	}
}
