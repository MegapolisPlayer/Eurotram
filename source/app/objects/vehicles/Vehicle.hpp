#ifndef EUROTRAM_OBJECTS_VEHICLES_VEHICLE
#define EUROTRAM_OBJECTS_VEHICLES_VEHICLE
#include "APEX.hpp"
#include "BUSE.hpp"

struct VehiclePhysicsData {
	float speed;

	//information
	float mass;
	float areaFront;
	float contactResistance;

	//forces
	float fceGravity;
	float fceFriction;
	float fceAerodynamic;
};

class Vehicle {
public:
	Vehicle() noexcept;
	~Vehicle() noexcept;
private:
	VehiclePhysicsData mData;
};

#endif
