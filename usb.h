#pragma once

#include <stdint.h>

extern bool isConnected;

/// -1 = idle, -2 = shooting preview, other = seconds countdown
extern uint8_t countdownValue;

void usbSetup();
void usbLoop();