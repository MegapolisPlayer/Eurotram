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
	float fceFront;
	float fceGravity;
	float fceFriction;
	float fceAerodynamic;
	float fceTurn;
	float nadal;

	//weather
	float power;
	float consumption;
};

//handles movement and stuff
class Vehicle {
public:
	Vehicle() noexcept;

	void physicsUpdate() noexcept;

	~Vehicle() noexcept;
private:
	VehiclePhysicsData mData;
};

#endif
