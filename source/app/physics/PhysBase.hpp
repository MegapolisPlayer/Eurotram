#ifndef EUROTRAM_PHYSICS_FRICTION
#define EUROTRAM_PHYSICS_FRICTION
#include "Weather.hpp"
#include "../objects/vehicles/Vehicle.hpp"

namespace Physics {
	float verticalForce(VehiclePhysicsData& aPhysData) noexcept;

	float perFrameSpeedFromAcceleration(float a) noexcept;

};

#endif
