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

	//offset - how far is bogie away from origin
	void setData(
		Mesh* aBogieName,
		Mesh* aBogieShaftSuffix1,
		Mesh* aBogieShaftSuffix2,
		const glm::vec3& aBogieOffset
	) noexcept;

	//updates station - for main bogie
	glm::vec2 move(Map* aMap, Line* aLine, const float aAmount, const bool aUpdateStation) noexcept;

	~BogieMovement() noexcept;
private:
	float mLengthRemaining;

	uint64_t mTrackId;
	std::pair<uint8_t, uint64_t> mNextNodeId;
	uint64_t mSwitchCount;

	Mesh* mBogie;
	Mesh* mShaft1;
	Mesh* mShaft2;
	glm::vec3 mBogieOffset;
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
	std::vector<BogieMovement> mBogies;
};

#endif
