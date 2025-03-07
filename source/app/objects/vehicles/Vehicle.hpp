#ifndef EUROTRAM_OBJECTS_VEHICLES_VEHICLE
#define EUROTRAM_OBJECTS_VEHICLES_VEHICLE
#include "APEX.hpp"
#include "BUSE.hpp"

struct VehicleInformation {
	float mass = 0.0;
	float areaFront = 0.0;
	float contactResistance = 0.0;

	//TODO add list of meshes which every bogie moves
	//something:
	//
	//body1 - 0,1
	//body2 - 1,2
	//
	//for other meshes - apply averages of first 2 bogies
	//
	//TODO validation
	//of all sizes
	//more or equal to 2 shafts
	//more or equal to 2 bogies
	//
	//TODO test on multi vehicle systems
	//add method for duplicating data

	Model* model;
	//for each bogie pair in bogieNames defined names of meshes
	//bogie pairs in format 0&1, 1&2, 2&3
	std::vector<std::vector<std::string>> meshNames;
	std::vector<std::string> bogieNames;
	//1 bogie = 2 shafts
	std::vector<std::string> bogieShaftSuffixes;
	std::vector<glm::vec3> bogieCenterOffset; //relative to center of model
	std::vector<float> bogieTrackOffset; //relative to previous, first value usually zero
};

struct VehiclePhysicsData {
	float speed;

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

class BogieMovement {
public:
	BogieMovement() noexcept;

	void setData(
		Model& aModel,
		const std::string_view aBogieName,
		const std::string_view aBogieShaftSuffix1,
		const std::string_view aBogieShaftSuffix2,
		const glm::vec3& aBogieOffset
	) noexcept;

	//offset - how far is bogie away from origin
	void setData(
		Mesh* aBogieName,
		Mesh* aBogieShaft1,
		Mesh* aBogieShaft2,
		const glm::vec3& aBogieOffset
	) noexcept;

	void init(Map& aMap, Line& aLine, const float aLengthRemainingOverride = 0.0) noexcept;

	//updates station - for main bogie
	std::optional<glm::vec3> move(Map& aMap, Line& aLine, const float aAmount, const bool aUpdateStation) noexcept;
	void applyMove(Map& aMap, Line& aLine) noexcept;

	bool isValid() const noexcept;
	void validate() const noexcept;

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

	glm::vec3 mPosition;
	glm::vec3 mRotation;
};

//TODO convert to instanced

//handles movement and stuff
class Vehicle {
public:
	Vehicle(Map& aMap, Line& aLine, VehicleInformation& aInfo) noexcept;

	//update data of vehicle
	void update(Map& aMap, Line& aLine) noexcept;
	void draw() noexcept;

	VehiclePhysicsData* getVehiclePhysicsData() noexcept;

	~Vehicle() noexcept;
private:
	Model* mModel;
	std::vector<std::vector<Mesh*>> mBogieMeshes;

	VehicleInformation mInfo;
	VehiclePhysicsData mPhysicsData;
	std::vector<BogieMovement> mBogies;
};

#endif
