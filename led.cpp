#include "led.h"

#include <stdint.h>
#include <Arduino.h>

#include "common.h"
#include "animation.h"

#define LED_PIN 0
#define NUMBYTES NUMPIXELS * 3

static uint8_t pixelsData[NUMBYTES];

float currentRgb[3];

#define H_60 0.166f // = 60 / 360
void hsv2Rgb(float h, float s, float v, float *rgb)
{
	h = fmod(h, 1);

	const float C = s * v;
	const float X = C * (1 - abs(fmod(h * 6, 2) - 1));
	const float m = v - C;
	
	float r, g, b;
	if (h < H_60)
		r = C, g = X, b = 0;

	else if (h < 2 * H_60)
		r = X, g = C, b = 0;

	else if (h < 3 * H_60)
		r = 0, g = C, b = X;

	else if (h < 4 * H_60)
		r = 0, g = X, b = C;

	else if (h < 5 * H_60)
		r = X, g = 0, b = C;

	else
		r = C, g = 0, b = X;

	rgb[0] = r + m;
	rgb[1] = g + m;
	rgb[2] = b + m;
}

void storeCurrentRgb(uint8_t pixelIx)
{
	uint8_t *pixelData = pixelsData + pixelIx * 3;
	pixelData[0] = static_cast<uint8_t>(currentRgb[1] * 255.0f);
	pixelData[1] = static_cast<uint8_t>(currentRgb[0] * 255.0f);
	pixelData[2] = static_cast<uint8_t>(currentRgb[2] * 255.0f);
}

void ledSetup()
{
	// Mark port 0 as output
	DDRB |= 1;

	// Write low
	PORTB &= ~1;
}

void ledLoop()
{
	static unsigned long lastMillis = 0;

	// Update max once per 100 ms
	if (currentMillis < lastMillis + 100)
		return;

	lastMillis = currentMillis;

	// Update pixel data
	{
		for (uint8_t i = 0; i < NUMPIXELS; i++)
		{
			hsv2Rgb((i + currentMillis / 64) * (1.0f / NUMPIXELS), 1.0f, 1.0f, currentRgb);
			storeCurrentRgb(i);
		}
	}

	// Upload the data
	{
		// Disable interrupts
		cli();

		volatile uint16_t
				i = NUMBYTES; // Loop counter

		volatile uint8_t
				*ptr = pixelsData, // Pointer to next byte
				b = *ptr++;				 // Current byte value

		const volatile uint8_t
				hi = PORTB | 1,	 // PORT w/output bit set high
				lo = PORTB & ~1; // PORT w/output bit set low

		volatile uint8_t
				next = lo,
				bit = 8;

		volatile uint8_t *port = &PORTB;

		asm volatile(
				"head20:\n\t"									 // Clk  Pseudocode    (T =  0)
				"st   %a[port],  %[hi]\n\t"		 // 2    PORT = hi     (T =  2)
				"sbrc %[byte],  7\n\t"				 // 1-2  if(b & 128)
				"mov  %[next], %[hi]\n\t"			 // 0-1   next = hi    (T =  4)
				"dec  %[bit]\n\t"							 // 1    bit--         (T =  5)
				"st   %a[port],  %[next]\n\t"	 // 2    PORT = next   (T =  7)
				"mov  %[next] ,  %[lo]\n\t"		 // 1    next = lo     (T =  8)
				"breq nextbyte20\n\t"					 // 1-2  if(bit == 0) (from dec above)
				"rol  %[byte]\n\t"						 // 1    b <<= 1       (T = 10)
				"rjmp .+0\n\t"								 // 2    nop nop       (T = 12)
				"nop\n\t"											 // 1    nop           (T = 13)
				"st   %a[port],  %[lo]\n\t"		 // 2    PORT = lo     (T = 15)
				"nop\n\t"											 // 1    nop           (T = 16)
				"rjmp .+0\n\t"								 // 2    nop nop       (T = 18)
				"rjmp head20\n\t"							 // 2    -> head20 (next bit out)
				"nextbyte20:\n\t"							 //                    (T = 10)
				"ldi  %[bit]  ,  8\n\t"				 // 1    bit = 8       (T = 11)
				"ld   %[byte] ,  %a[ptr]+\n\t" // 2    b = *ptr++    (T = 13)
				"st   %a[port], %[lo]\n\t"		 // 2    PORT = lo     (T = 15)
				"nop\n\t"											 // 1    nop           (T = 16)
				"sbiw %[count], 1\n\t"				 // 2    i--           (T = 18)
				"brne head20\n"								 // 2    if(i != 0) -> (next byte)
				: [port] "+e"(port),
					[byte] "+r"(b),
					[bit] "+r"(bit),
					[next] "+r"(next),
					[count] "+w"(i)
				: [ptr] "e"(ptr),
					[hi] "r"(hi),
					[lo] "r"(lo));

		sei();
	}
}
