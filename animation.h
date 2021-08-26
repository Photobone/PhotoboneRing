#pragma once

enum class Animation {
	connected,
	idle,
	shooting,
};

void updateAnimation(Animation a);