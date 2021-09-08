#pragma once

#include <stdint.h>

#define NUMPIXELS 24

void ledSetup();
void ledLoop();

extern float currentRgb[3];

/// Store correntRgb as color of pixel IX
void storeCurrentRgb(uint8_t pixelIx);

void hsv2Rgb(float h, float s, float v, float *rgb);
