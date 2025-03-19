#ifndef EUROTRAM_PHYSICS_FRICTION
#define EUROTRAM_PHYSICS_FRICTION
#include "Weather.hpp"

namespace Physics {
	float perFrameSpeedFromAcceleration(float a, float t) noexcept;
	float accelerationFromForce(float f, float m) noexcept;

	//throttle between 0 and 1, engine rotation in RPM, wheel diameter in meters
	float forceFromPower(float pow, float throttle, float engineRot, float gearRatio, float velocity, float wheelDiameter, float efficiency = 0.95) noexcept;

	float verticalForce(float v, float l, float angle) noexcept;
};

#endif
