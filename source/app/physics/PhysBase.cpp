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

		return 0.0;
	}
	float verticalForce(float v, float l, float angle) noexcept {
		return 0.0; //TODO
	}
}
