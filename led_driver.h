#pragma once


typedef enum ANIMATION { 
  
  POWER_UP, 
  COMM_ERR,
  
  PHOTO_IDLE, 
  PHOTO_TIMER
  
} ANIMATION;



#include "color_conv.h"

#include <Adafruit_NeoPixel.h>
#define LED_PIN        0
#define NUMPIXELS      24


// Color order is in fact: GRB
Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_RGB + NEO_KHZ800);





/*private*/ void systemFlash(long progress, uint32_t color) {
  const int UP_DELAY_MS = 13;
  const int ALL_ON_MS = 100;
  const int TWI_MS = 150;
  

  int twiStart = UP_DELAY_MS * NUMPIXELS + ALL_ON_MS;
  if (progress < twiStart) {
    for(int i = 0; i < NUMPIXELS; i++) {
      bool on = progress > i * UP_DELAY_MS;
      
      ring.setPixelColor(i, color * on);
    }

  } else {
    float factor = max(0, (float)(TWI_MS - (progress - twiStart)) / TWI_MS);

    uint8_t r = factor * ((color >> 16) & 0xFF);
    uint8_t g = factor * ((color >>  8) & 0xFF);
    uint8_t b = factor * ((color >>  0) & 0xFF);

    
    for(int i = 0; i < NUMPIXELS; i++) {      
      ring.setPixelColor(i, ring.Color(r, g, b));
    }
  }

}

/*private*/ void photoIdleLoop(long progress, uint8_t brightness) {
  const int ROTATION_DELAY = 50;
//  const int ROTATION_ALPHA_DELAY = 150;


  for(int i = 0; i < NUMPIXELS; i++) {

    int h = ((int)((i + (double)progress / ROTATION_DELAY) * 360 / NUMPIXELS)) % 360;
//    float alpha = (i + (double)progress / ROTATION_DELAY) / NUMPIXELS / 2 + 0.5f;
    
    uint8_t r, g, b;

    HSVtoRGB(h, 100, brightness, &r, &g, &b);
    
    ring.setPixelColor(i, ring.Color(g * 1.35f * alpha, r * 3 * alpha, b * alpha));
  }
    
  
}


/*private*/ void photoTimerLoop(long progress, uint8_t brightness) {
  const int ROTATION_DELAY_START = 100;
  const int ROTATION_DELAY_END = 25;

  const int ROTATION_SPEEDUP_DURATION_MS = 2000;
  
  const int BLINK_DURATION_MS = 2000;
  const int BLINK_INTERVAL_MS = 150;


  if (progress < ROTATION_SPEEDUP_DURATION_MS) {

    int rotationDelay = (ROTATION_DELAY_START - ROTATION_DELAY_END) * (ROTATION_SPEEDUP_DURATION_MS - progress) / ROTATION_SPEEDUP_DURATION_MS + ROTATION_DELAY_END;

    uint8_t r, g, b;
    int bri = brightness
        * progress * 3 / ROTATION_SPEEDUP_DURATION_MS;
        
    for(int i = 0; i < NUMPIXELS; i++) {
  
      int h = ((int)((i + (double)progress / rotationDelay) * 360 / NUMPIXELS)) % 360;
 
  
      HSVtoRGB(h, 100, bri, &r, &g, &b);
      
      ring.setPixelColor(i, ring.Color(g * 1.35f, r * 3, b));
    }
    
  } else if (progress < BLINK_DURATION_MS + ROTATION_SPEEDUP_DURATION_MS) {

    uint32_t color = 0;
    if (((progress - ROTATION_SPEEDUP_DURATION_MS) / BLINK_INTERVAL_MS) % 2) {
        color = ring.Color(255, 255, 255);
    }
    
    for(int i = 0; i < NUMPIXELS; i++) {
      ring.setPixelColor(i, color);
    }
    
  } else {
    for(int i = 0; i < NUMPIXELS; i++) {
      ring.setPixelColor(i, 0);
    }
  }

  
}





void ledSetup() {
    ring.begin();
}

void ledLoop(ANIMATION anim, long progress) {

  switch(anim) {
    case POWER_UP: 
      systemFlash(progress, ring.Color(150, 0, 0));
      break;
    case COMM_ERR: 
      systemFlash(progress, ring.Color(0, 150, 0));
      break;

    case PHOTO_IDLE:
      photoIdleLoop(progress, 5);
      break;
    case PHOTO_TIMER:
      photoTimerLoop(progress, 5);
      break;
  }

  ring.show();

}
