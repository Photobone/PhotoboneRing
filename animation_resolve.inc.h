
inline ANIMATION resolveAnimation()
{
	if (currentMillis < 1500)
	{
		return POWER_UP;
	}

	if (!isConnected)
	{
		return COMM_ERR;
	}

	if (countdownValue < 5)
		return PHOTO_TIMER;

	return PHOTO_IDLE;
}