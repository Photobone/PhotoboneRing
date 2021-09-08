uint8_t countdownValue = 0;

void transmitData()
{
	// Signal begin of packet - low clock, change data, high clock
	PORTB &= ~_BV(PB1);
	delayMicroseconds(500);

	PORTB ^= _BV(PB0);
	PORTB |= _BV(PB1);
	delayMicroseconds(500);

	uint8_t data = -1;//countdownValue;
	for (uint8_t i = 0; i < 8; i++)
	{
		// Set the data output
		PORTB = (PORTB & ~1) | (data >> 7);

		// Clock low
		PORTB &= ~_BV(PB1);
		delayMicroseconds(500);

		// Clock high
		PORTB |= _BV(PB1);
		delayMicroseconds(500);

		// Shift data
		data <<= 1;
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