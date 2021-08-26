#include "animation.h"
#include "led.h"


//extern float currentRgb[3];
//void storeColor(uint8_t pixelIx);
//void hsv2Rgb(float h, float s, float v, float *rgb);




float[] GREEN = { 0.0f, 0.0f, 1.0f };
float[] RED = { 1.0f, 0.0f, 0.0f };
float[] WHITE = { 1.0f, 1.0f, 1.0f };
float[] BLACK = { 0.0f, 0.0f, 0.0f };



/*private*/ void systemFlash(long progress, float *rgb) {
  const int UP_DELAY_MS = 13;
  const int ALL_ON_MS = 100;
  const int TWI_MS = 150;
  

  int twiStart = UP_DELAY_MS * NUMPIXELS + ALL_ON_MS;
  if (progress < twiStart) {
    memcpy(currentRgb, rgb, sizeof(float) * 3);
    
    for(int i = 0; i < NUMPIXELS; i++) {
      if (progress < i * UP_DELAY_MS) {
        currentRgb[0] = 0.0f;
        currentRgb[1] = 0.0f;
        currentRgb[2] = 0.0f;
      }

      
      storeColor(i);
    }

  } else {
    float factor = max(0, (float)(TWI_MS - (progress - twiStart)) / TWI_MS);

    currentRgb[0] = rgb[0] * factor;
    currentRgb[1] = rgb[1] * factor;
    currentRgb[2] = rgb[2] * factor;

    for(int i = 0; i < NUMPIXELS; i++) {      
      storeColor(i);
    }
  }

}

/*private*/ void photoIdleLoop() {
  const int ROTATION_DELAY = 350;
  const int ROTATION_ALPHA_X = 3;


  long progressMod = currentMillis % (ROTATION_DELAY * ROTATION_ALPHA_X * NUMPIXELS);

  for(int i = 0; i < NUMPIXELS; i++) {

    float h = ((double)i + (double)progressMod / ROTATION_DELAY) / NUMPIXELS;
    while (h > 360) h -= 360;
    
//    float alpha = (i + (double)progressMod / ROTATION_DELAY / ROTATION_ALPHA_X) / NUMPIXELS;
//    while (alpha > 1) alpha -= 1;
//    alpha = max(0, abs(alpha - 0.5f) * 6 - 2);


    hsv2Rgb(h, 1.0f, 0.1f, currentRgb);
    currentRgb[0] *= 3;
    currentRgb[1] *= 1;
    currentRgb[2] *= 1.35f;
    
    storeColor(i);
  }    
  
}


/*private*/ void photoTimerLoop() {
  const int ROTATION_DELAY_START = 100;
  const int ROTATION_DELAY_END = 25;

  const int ROTATION_SPEEDUP_DURATION_MS = 2000;
  
  const int BLINK_DURATION_MS = 2000;
  const int BLINK_INTERVAL_MS = 150;


  if (progress < ROTATION_SPEEDUP_DURATION_MS) {

    int rotationDelay = (ROTATION_DELAY_START - ROTATION_DELAY_END) * (ROTATION_SPEEDUP_DURATION_MS - progress) / ROTATION_SPEEDUP_DURATION_MS + ROTATION_DELAY_END;

    uint8_t r, g, b;
    float bri = 0.1f * progress * 3 / ROTATION_SPEEDUP_DURATION_MS;
        
    for(int i = 0; i < NUMPIXELS; i++) {
  
      float h = ((double)i + (double)progress / rotationDelay) / NUMPIXELS;
      while (h > 360) h -= 360;
 
      hsv2Rgb(h, 1.0f, 0.1f, currentRgb);
      currentRgb[0] *= 3;
      currentRgb[1] *= 1;
      currentRgb[2] *= 1.35f;
      
      storeColor(i);
    }
    
  } else if (progress < BLINK_DURATION_MS + ROTATION_SPEEDUP_DURATION_MS) {

    if (((progress - ROTATION_SPEEDUP_DURATION_MS) / BLINK_INTERVAL_MS) % 2) {
      memcpy(currentRgb, WHITE, sizeof(float) * 3);
    } else {
      memcpy(currentRgb, BLACK, sizeof(float) * 3);  
    }
    
    
    for(int i = 0; i < NUMPIXELS; i++) {
      storeColor(i);
    }
    
  } else {
    memcpy(currentRgb, BLACK, sizeof(float) * 3);
    
    for(int i = 0; i < NUMPIXELS; i++) {
      storeColor(i);
    }
  }

  
}


ANIMATION lastAnimation = NULL_ANIMATION;
long lastAnimationStart = 0
void updateAnimation(ANIMATION anim) {

  if (lastAnimation != anim) {
    lastAnimation = anim;
    lastAnimationStart = currentMillis;
  }
  long sinceStart = currentMillis - lastAnimationStart;
  

  switch(anim) {
    case POWER_UP: 
      systemFlash(sinceStart, GREEN);
      break;
    case COMM_ERR: 
      systemFlash(sinceStart, RED);
      break;

    case PHOTO_IDLE:
      photoIdleLoop(progress);
      break;
    case PHOTO_TIMER:
      photoTimerLoop(progress);
      break;
  }

  ring.show();

}
