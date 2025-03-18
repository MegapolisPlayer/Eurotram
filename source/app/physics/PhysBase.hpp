#ifndef EUROTRAM_PHYSICS_FRICTION
#define EUROTRAM_PHYSICS_FRICTION
#include "Weather.hpp"

namespace Physics {
	float perFrameSpeedFromAcceleration(float a, float t) noexcept;
	float accelerationFromForce(float f, float m) noexcept;
	float forceFromPower(float pow, float t, float throttle, float velocity) noexcept; //throttle between 0 and 1
	float verticalForce(float v, float l, float angle) noexcept;
};

#endif
