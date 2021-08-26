#pragma once

typedef enum ANIMATION { 

  NULL_ANIMATION=0,
  
  POWER_UP, 
  COMM_ERR,
  
  PHOTO_IDLE, 
  PHOTO_TIMER
  
} ANIMATION;


void updateAnimation();
