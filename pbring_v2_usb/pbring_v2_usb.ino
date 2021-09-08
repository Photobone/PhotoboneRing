uint8_t countdownValue = 0;

void transmitData()
{
	uint8_t data = countdownValue;
	for (uin8_t i = 0; i < 8; i++)
	{
		PORTB = (PORTB & ~1) | ((data >> 7) & 1);
		PORTB ^= _BV(PB1);
	}
}

void setup()
{
	// Pins 0 and 1 set as outputs
	DDRB = _BV(PB0) | _BV(PB1);
}

void loop()
{
	countdownValue = 4 * 8 - (millis() / (1000 / 8)) % 5 * 8;

	transmitData();
}