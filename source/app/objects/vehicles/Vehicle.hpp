#ifndef EUROTRAM_OBJECTS_VEHICLES_VEHICLE
#define EUROTRAM_OBJECTS_VEHICLES_VEHICLE
#include "APEX.hpp"
#include "BUSE.hpp"

#include "../../physics/Physics.hpp"

struct VehicleInformation {
	float mass = 0.0, length = 0.0, width = 0.0, height = 0.0;
	uint64_t seats, standing;

	float areaFront = 0.0;
	float aerodynamicCoef = 0.0;
	float gearRatio = 0.0, wheelDiameter = 0.0, motorRPM = 0.0, motorAmount = 0.0;

	std::string modelFile;
	//for each bogie pair in bogieNames defined names of meshes
	//bogie pairs in format 0&1, 1&2, 2&3
	std::vector<std::vector<std::string>> meshNames;
	std::vector<std::string> bogieNames;
	//1 bogie = 2 shafts
	std::vector<std::string> bogieShaftSuffixes;
	std::vector<glm::vec3> bogieCenterOffset; //relative to center of model
	std::vector<float> bogieTrackOffset; //relative to previous, first value usually zero

	std::string variantMaterial;
	std::vector<std::pair<std::string, std::string>> variants; //for liveries, pair <variantName, variantTexture>

	glm::vec3 cameraOffset;

	bool validate() noexcept;
};

struct VehicleCabinTriggerDataEntry {
	glm::vec3 offset;
	glm::vec3 size = glm::vec3(0.1);
	float rotation;
};

struct VehicleCabinTriggerData {
	enum VEHICLE_CABIN_TRIGGER_DATA_ID : uint16_t {
		STARTER = 0, SANDER, STOP,
		CONTROL_AMOUNT //must be last!
	};

	std::vector<VehicleCabinTriggerDataEntry> data;
	//TODO
};

#define ENUM_TO_INT(x) int64_t(x)

struct VehicleControlData {
	float throttle = 0.0;

	float brakeDynamic = 0.0; //stop motors, makes then regenerate electricity
	float brakeMechanic = 0.0; //stops motors using friction - when under 5km/h
	float brakeEmergency = 0.0; //physical, adds much more friction, EMERGENCY BRAKE

	bool sander = false;
	bool pantographContact = true; //TODO
};

struct VehiclePhysicsData {
	float frictionCoef = 0.0;

	float speed = 0.0; //per 1/50 of a second
	float physicsSpeed = 0.0; //in meters per second
	float maxSpeed = 0.0;
	float acceleration = 0.0;

	//forces
	float fceResult = 0.0;

	float fceVertical = 0.0;
	float verticalAcceleration = 0.0;
	float verticalSpeed = 0.0;
	float verticalDistanceTravelled = 0.0; //for derailments

	float fceFront = 0.0;
	float fceGravity = 0.0;
	float fceNormal = 0.0;
	float fceResistance = 0.0; //rolling resistance
	float fceFriction = 0.0;
	float fceAerodynamic = 0.0;
	float fceTurn = 0.0;

	float lvr = 0.0; //actual
	float nadal = 0.0; //limit

	//weather
	float power = 0.0; //final power = power*motorAmount

	//electricity - just for display
	float voltage = 0.0;

	//counters
	float energyUsed = 0.0;
};

class BogieMovement {
	friend class Vehicle;
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
	std::vector<std::pair<uint8_t, uint64_t>> mPassedNodes;

	Mesh* mBogie;
	Mesh* mShaft1;
	Mesh* mShaft2;
	glm::vec3 mBogieOffset;

	glm::vec3 mPosition;
	glm::vec3 mRotation;
};

//TODO convert to instanced

class Vehicle;

namespace UI {
	void drawPhysicsInfoWindow(Vehicle& aVehicle) noexcept;
}

//handles movement and stuff
class Vehicle {
	friend void UI::drawPhysicsInfoWindow(Vehicle& aVehicle) noexcept;
public:
	Vehicle(Map& aMap, Line& aLine, VehicleInformation& aInfo, Model* aModel) noexcept;

	//call init separately
	Vehicle(const std::string_view aConfigFilename) noexcept;

	Vehicle(Vehicle& aOther) noexcept = delete;
	Vehicle& operator=(Vehicle& aOther) noexcept = delete;

	Vehicle(Vehicle&& aOther) noexcept;
	Vehicle& operator=(Vehicle&& aOther) noexcept;

	std::string getConfigModelFilename() const noexcept;
	void init(Map& aMap, Line& aLine, Model* aModel) noexcept;

	//update data of vehicle
	void update(Map& aMap, Line& aLine) noexcept;

	void physicsUpdate(const uint16_t aWeather, const float aPhysicsUpdateFreq) noexcept;

	//returns false if over speed limit
	bool setSpeed(const float aSpeed) noexcept;

	VehicleControlData* getVehicleControlData() noexcept;
	VehiclePhysicsData* getVehiclePhysicsData() noexcept;
	glm::vec3 getCameraPosition() const noexcept;
	glm::vec3 getCameraRotation() const noexcept; //of first section, used for camera

	BoxTriggerDrawer& getControlsDraw() noexcept;
	std::vector<BoxTrigger>& getTriggers() noexcept;

	bool isEmergencyBraking() noexcept;

	~Vehicle() noexcept;

	//returns filename of model file
	static std::string loadVehicleFromConfigurationFile(Vehicle& aVehicle, const std::string_view aFilename) noexcept;
private:
	Model* mModel;
	std::vector<std::vector<Mesh*>> mBogieMeshes;

	VehicleInformation mInfo;
	VehiclePhysicsData mPhysicsData;
	std::vector<BogieMovement> mBogies;

	VehicleControlData mControlData;
	VehicleCabinTriggerData mCabinData;
	BoxTriggerDrawer mTriggerDrawer;
	std::vector<BoxTrigger> mTriggers;

	glm::vec3 mCameraLocation;
	glm::vec3 mCameraRotation; //physics data has own data
};

#endif
