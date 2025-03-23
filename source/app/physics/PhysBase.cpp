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
		if(l == 0.0) return 0.0;
		glm::vec2 r = glm::vec2(v, l);
		float alpha = Math::getRotationOfVector2DY(r);
		//first cosine - get Ftan, second - get vertical part of it
		return glm::cos(glm::radians(90 - angle)) * glm::length(r) * std::cos(glm::radians(alpha - angle));
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
	float forceTurn(float mass, float speed, float radius) noexcept {
		if(radius == 0.0) return 0.0;
 		//F = ma
		//a = v2/r
		return std::abs(mass * speed * speed) / std::abs(radius);
	}

	float maxResistanceForce(float speed, float mass, float time) noexcept {
		//cannot be larger than force to brake - this is the force to bring tram to halt in 1 tick
		//v/t = a
		//v = at   //*t
		//v = Ft/m
		//vm = Ft  //*m
		//vm/t = F  // /t
		//does make sense: amount of newtons to make acceleration equal current speed (remove it in 1 tick)
		//units fit:  m/s * kg / s = kgm/s2 = N
		return speed*mass/time;
	}

	float resultingForce(float forward, float resistance, float maxResistance, float velocity) noexcept {
		//if velocity positive -> multiply by 1
		//if velocity negative -> reverse also resistance strength
		return forward - ((velocity >= -0.005)*2-1) * std::min(std::abs(resistance), std::abs(maxResistance));
	}

	float powerConsumed(float pow, float throttle) noexcept {
		return 0.0;
	}
}
