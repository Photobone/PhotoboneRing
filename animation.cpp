#include "animation.h"
#include "led.h"
#include "common.h"


float GREEN[] = { 0.0f, 1.0f, 0.0f };
float RED[] = { 1.0f, 0.0f, 0.0f };
float WHITE[] = { 1.0f, 1.0f, 1.0f };
float BLACK[] = { 0.0f, 0.0f, 0.0f };



/*private*/ void systemFlash(long progress, float *rgb) {
  const int UP_DELAY_MS = 13;
  const int ALL_ON_MS = 100;
  const int TWI_MS = 150;
  

  int twiStart = UP_DELAY_MS * NUMPIXELS + ALL_ON_MS;
  if (progress < twiStart) {
    currentRgb[0] = rgb[0];
    currentRgb[1] = rgb[1];
    currentRgb[2] = rgb[2];
    
    
    for(int i = 0; i < NUMPIXELS; i++) {
      if (progress < i * UP_DELAY_MS) {
        currentRgb[0] = 0.0f;
        currentRgb[1] = 0.0f;
        currentRgb[2] = 0.0f;
      }

      
      storeCurrentRgb(i);
    }

  } else {
    float factor = (TWI_MS - (progress - twiStart)) / TWI_MS;
    factor = factor > 0 ? factor : 0;

    currentRgb[0] = rgb[0] * factor;
    currentRgb[1] = rgb[1] * factor;
    currentRgb[2] = rgb[2] * factor;

    for(int i = 0; i < NUMPIXELS; i++) {      
      storeCurrentRgb(i);
    }
  }

}

/*private*/ void photoIdleLoop() {
  const int ROTATION_DELAY = 250;
  const int ROTATION_ALPHA_X = 3;


  uint32_t progressMod = currentMillis % (ROTATION_DELAY * ROTATION_ALPHA_X * NUMPIXELS);

  for(uint8_t i = 0; i < NUMPIXELS; i++) {

    float h = (i + (float)progressMod / ROTATION_DELAY) / NUMPIXELS;
    while (h > 360) h -= 360;
    
    float alpha = (i + (float)progressMod / ROTATION_DELAY * ROTATION_ALPHA_X) / NUMPIXELS;
    while (alpha > 1) alpha -= 1;
    alpha = alpha - 0.5f;
    alpha = alpha > 0 ? alpha : -alpha;
    alpha = alpha * 5 - 1.5f;
    alpha = alpha > 0 ? alpha : 0;


    hsv2Rgb(h, 1.0f, 0.05f * alpha, currentRgb);
    currentRgb[0] *= 3;
    currentRgb[1] *= 1;
    currentRgb[2] *= 1.35f;
    
    storeCurrentRgb(i);
  }    
  
}


/*private*/ void photoTimerLoop(long progress) {
  const int ROTATION_DELAY_START = 200;
  const int ROTATION_DELAY_END = 10;

  const int ROTATION_SPEEDUP_DURATION_MS = 3500;
  
  const int BLINK_DURATION_MS = 3000;
  const int BLINK_INTERVAL_MS = 150;


  if (progress < ROTATION_SPEEDUP_DURATION_MS) {

    int rotationDelay = (ROTATION_DELAY_START - ROTATION_DELAY_END) * (ROTATION_SPEEDUP_DURATION_MS - progress) / ROTATION_SPEEDUP_DURATION_MS + ROTATION_DELAY_END;

    uint8_t r, g, b;
    float bri = 0.05f * progress * 3 / ROTATION_SPEEDUP_DURATION_MS;
        
    for(uint8_t i = 0; i < NUMPIXELS; i++) {
  
      float h = (i + (double)progress / rotationDelay) / NUMPIXELS;
      while (h > 360) h -= 360;
 
      hsv2Rgb(h, 1.0f, 0.1f, currentRgb);
      currentRgb[0] *= 3;
      currentRgb[1] *= 1;
      currentRgb[2] *= 1.35f;
      
      storeCurrentRgb(i);
    }
    
  } else if (progress < BLINK_DURATION_MS + ROTATION_SPEEDUP_DURATION_MS) {

    if (((progress - ROTATION_SPEEDUP_DURATION_MS) / BLINK_INTERVAL_MS) % 2) {
      currentRgb[0] = WHITE[0] * 0.6f;
      currentRgb[1] = WHITE[1] * 0.6f;
      currentRgb[2] = WHITE[2] * 0.6f;
    } else {
      currentRgb[0] = BLACK[0];
      currentRgb[1] = BLACK[1];
      currentRgb[2] = BLACK[2];
    }
    
    
    for(int i = 0; i < NUMPIXELS; i++) {
      storeCurrentRgb(i);
    }
    
  } else {
    currentRgb[0] = BLACK[0];
    currentRgb[1] = BLACK[1];
    currentRgb[2] = BLACK[2];
    
    for(uint8_t i = 0; i < NUMPIXELS; i++) {
      storeCurrentRgb(i);
    }
  }

  
}


ANIMATION resolveAnimation() {

  if (currentMillis < 1500) {
    return POWER_UP;
  } else if (currentMillis < 10000) {
    return PHOTO_IDLE;
  } else if (currentMillis < 20000) {
    return PHOTO_TIMER;
  } else {
    return PHOTO_IDLE;
  }
  
}



ANIMATION lastAnimation = NULL_BLACK;
long lastAnimationStart = 0;
void updateAnimation() {

  ANIMATION currentAnim = resolveAnimation();

  if (lastAnimation != currentAnim) {
    lastAnimation = currentAnim;
    lastAnimationStart = currentMillis;
  }
  long sinceStart = currentMillis - lastAnimationStart;
  

  switch(currentAnim) {
    case POWER_UP: 
      systemFlash(sinceStart, GREEN);
      break;
    case COMM_ERR: 
      systemFlash(sinceStart, RED);
      break;

    case PHOTO_IDLE:
      photoIdleLoop();
      break;
    case PHOTO_TIMER:
      photoTimerLoop(sinceStart);
      break;

    case NULL_BLACK:
      currentRgb[0] = BLACK[0];
      currentRgb[1] = BLACK[1];
      currentRgb[2] = BLACK[2];
    
      for(uint8_t i = 0; i < NUMPIXELS; i++) {
        storeCurrentRgb(i);
      }
      break;
      
  }

}
