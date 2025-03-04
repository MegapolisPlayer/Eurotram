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

	float lengthBetweenBogies;
	std::vector<std::string> bogieNames;
	//1 bogie = 2 shafts
	std::vector<std::string> bogieShaftNames;
};

class BogieMovement {
public:
	BogieMovement() noexcept;

	//updates station - for main bogie
	void moveLead(Line* aLine, const float aAmount) noexcept;
	//doesnt update station - for "follower" bogies
	void moveFollow(Line* aLine, const float aAmount) noexcept;

	~BogieMovement() noexcept;
private:
	float mLengthRemaining;

	uint64_t mTrackId;
	std::pair<uint8_t, uint64_t> mNextNodeId;
	uint64_t mSwitchCount;

	std::string mBogieShaft1;
	std::string mBogieShaft2;
	std::string mBogieName;
};


//handles movement and stuff
class Vehicle {
public:
	Vehicle() noexcept;

	void movementUpdate() noexcept;

	void physicsUpdate() noexcept;

	//TODO getters

	~Vehicle() noexcept;
private:
	Map* mMap;
	Line* mLine;

	VehiclePhysicsData mData;
	std::vector<BogieMovement> mMovementData;
};

#endif
