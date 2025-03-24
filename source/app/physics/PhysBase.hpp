#ifndef EUROTRAM_PHYSICS_FRICTION
#define EUROTRAM_PHYSICS_FRICTION
#include "Weather.hpp"

namespace Physics {
	float perFrameSpeedFromAcceleration(float a, float t) noexcept;
	float accelerationFromForce(float f, float m) noexcept;

	//throttle between 0 and 1, engine rotation in RPM, wheel diameter in meters
	float forceFromPower(float pow, float throttle, float engineRot, float gearRatio, float velocity, float wheelDiameter, float efficiency = 0.95) noexcept;

	//angle picked arbitrarily
	float nadalLimit(float frictionCoef, float angle = 30.0) noexcept;
	float verticalForce(float v, float l, float angle = 30.0) noexcept;

	float forceGravity(float m, float g = 9.81) noexcept;
	float forceNormal(float forceGravity, float angle) noexcept;
	float forceFriction(float normal, float coef) noexcept;
	float forceRollingResistance(float normal, float coef = 0.001) noexcept;
	float forceAerodynamic(float speed, float frontArea, float aerodynamicCoef) noexcept;
	float forceTurn(float mass, float speed, float radius) noexcept;

	float maxResistanceForce(float speed, float mass, float time) noexcept;
	float resultingForce(float forward, float resistance, float maxResistance, float velocity) noexcept;

	float current(float pow, float voltage = 600, float phaseAngleRad = 0.9) noexcept;
	float powerConsumed(float pow, float throttle, float time) noexcept;
};

#endif
