#pragma once

#define NUMPIXELS 24

void ledSetup();
void ledLoop();

extern float currentRgb[3];

void storeColor(uint8_t pixelIx);

void hsv2Rgb(float h, float s, float v, float *rgb);
