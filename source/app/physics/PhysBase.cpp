#include "PhysBase.hpp"

namespace Physics {
	float perFrameSpeedFromAcceleration(float a, float t) noexcept {
		//a = vt => v = a/t
		return a/t;
	}
	float accelerationFromForce(float f, float m) noexcept {
		//2nd newton's law => F = ma => a = F/m
		return f/m;
	}
	float forceFromPower(float pow, float t, float throttle, float velocity) noexcept {
		//TODO

		return 0.0;
	}
	float verticalForce(float v, float l, float angle) noexcept {
		return 0.0; //TODO
	}
}
