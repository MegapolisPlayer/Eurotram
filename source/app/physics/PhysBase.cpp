#include "PhysBase.hpp"

namespace Physics {
	float perFrameSpeedFromAcceleration(float a, float t) noexcept {
		//a = v/t => v = at
		return a*t;
	}
	float accelerationFromForce(float f, float m) noexcept {
		//2nd newton's law => F = ma => a = F/m
		return f/m;
	}
	float forceFromPower(float pow, float throttle, float engineRot, float gearRatio, float velocity, float wheelDiameter, float efficiency) noexcept {
		if(velocity < 3.0) {
			//calculate starting tractive effort (from torque)

			//torque: P = wM => M = P/w

			//rad/s: RPM to RP/sec, RP/sec to deg/sec, convert to rads
			float w = (engineRot/60*360)*std::numbers::pi/180.0;

			//torque
			float m = pow*throttle/w;

			//tractive effort: moment * efficiency * gear ratio / wheel radius
			return m*efficiency*gearRatio/(wheelDiameter/2.0);
		}
		else {
			//calculate "normal" tractive effort (P = Fv) => F = P/v
			return pow*throttle/velocity;
		}
	}
	float nadalLimit(float frictionCoef, float angle) noexcept {
		return (std::tan(glm::radians(angle)) - frictionCoef) / (1.0 + frictionCoef * std::tan(glm::radians(angle)));
	}
	float verticalForce(float v, float l, float angle) noexcept {


	}

	float forceGravity(float m, float g) noexcept {
		return m*g;
	}
	float forceNormal(float forceGravity, float angle) noexcept {
		return forceGravity*std::cos(glm::radians(angle));
	}
	float forceFriction(float normal, float coef) noexcept {
		return normal*coef;
	}
	float forceRollingResistance(float normal, float coef) noexcept {
		return normal*coef;
	}
	float forceAerodynamic(float speed, float frontArea, float aerodynamicCoef) noexcept {
		//1/2pv2cS
		return 0.5 * speed * speed * aerodynamicCoef * frontArea;
	}
}
