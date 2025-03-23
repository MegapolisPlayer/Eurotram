#include "Vehicle.hpp"

bool VehicleInformation::validate() noexcept {
	return
		this->bogieCenterOffset.size() == this->bogieTrackOffset.size() &&
		this->bogieShaftSuffixes.size() == 2 &&
		(this->bogieNames.size() == this->meshNames.size()-1 || this->meshNames.empty()) &&
		this->bogieNames.size() >= 1;
}

BogieMovement::BogieMovement() noexcept
	: mBogie(nullptr), mShaft1(nullptr), mShaft2(nullptr), mLengthRemaining(0.0), mTrackId(0), mSwitchCount(0) {}

	void BogieMovement::setData(
	Model& aModel,
	const std::string_view aBogieName,
	const std::string_view aBogieShaftSuffix1,
	const std::string_view aBogieShaftSuffix2,
	const glm::vec3& aBogieOffset
) noexcept {
	this->mBogie = aModel.getMesh(aBogieName);
	this->mShaft1 = aModel.getMesh(std::string(aBogieName).append(aBogieShaftSuffix1));
	this->mShaft2 = aModel.getMesh(std::string(aBogieName).append(aBogieShaftSuffix2));
	this->mBogieOffset = aBogieOffset;
}
void BogieMovement::setData(
	Mesh* aBogieName,
	Mesh* aBogieShaft1,
	Mesh* aBogieShaft2,
	const glm::vec3& aBogieOffset
) noexcept {
	this->mBogie = aBogieName;
	this->mShaft1 = aBogieShaft1;
	this->mShaft2 = aBogieShaft2;
	this->mBogieOffset = aBogieOffset;
}

void BogieMovement::init(Map& aMap, Line& aLine, const float aLengthRemainingOverride) noexcept {
	this->mTrackId = aLine.getFirstLoopTrack();
	this->mNextNodeId = aMap.getOtherTrackPoint(this->mTrackId, aLine.getFirstNodePassed());
	this->mSwitchCount = 0;
	this->mLengthRemaining = aLengthRemainingOverride;
}

//doesnt have any check - responsibilty of the caller
std::optional<glm::vec3> BogieMovement::move(Map& aMap, Line& aLine, const float aAmount, const bool aUpdateStation) noexcept {
	this->mLengthRemaining -= aAmount;

	//update track

	if(this->mLengthRemaining <= 0) {
		if(this->mSwitchCount >= aLine.getSwitchesToNextStop().size()) {
			//we already passed last switch - no more setting of value (if reach switch - game over)
			this->mTrackId = aMap.getNextTrack(this->mTrackId, this->mNextNodeId, LineData::SwitchDirection::NO_SET);
		}
		else {
			this->mTrackId = aMap.getNextTrack(this->mTrackId, this->mNextNodeId, aLine.getSwitchesToNextStop()[this->mSwitchCount].direction);
		}
		if(this->mNextNodeId.first == 's') {
			this->mSwitchCount++;
		}
		this->mNextNodeId = aMap.getOtherTrackPoint(this->mTrackId, this->mNextNodeId);
		if(aMap.isTrackStation(this->mTrackId)) {
			this->mSwitchCount = 0;
			if(aUpdateStation) {
				aLine.nextStation();
			}
		}
		this->mLengthRemaining += aMap.getTrackById(this->mTrackId).length;
	}
	else if(this->mLengthRemaining >= aMap.getTrackById(this->mTrackId).length) {
		this->mLengthRemaining -= aMap.getTrackById(this->mTrackId).length;
		//reverse
		auto sharedPoint = aMap.getOtherTrackPoint(this->mTrackId, this->mNextNodeId);
		if(sharedPoint.first == 's') {
			//cannot back into switch
			std::cerr << LogLevel::ERROR << "Cannot back into switch!\n" << LogLevel::RESET;
			return {};
		}
		this->mTrackId = aMap.getNextTrack(this->mTrackId, sharedPoint, aLine.getSwitchesToNextStop()[this->mSwitchCount].direction);
		this->mNextNodeId = sharedPoint; //move point back
	}

	//get position of bogies

	Track& tr = aMap.getTrackById(this->mTrackId);
	if(
		(int32_t)this->mNextNodeId.second == tr.first &&
		(
			((tr.flags & TRACK_FLAG_FIRST_SWITCH) && this->mNextNodeId.first == 's') ||
			(!(tr.flags & TRACK_FLAG_FIRST_SWITCH) && this->mNextNodeId.first == 'n')
		)
	) {
		//to first
		this->mPosition = tr.getPosition((this->mLengthRemaining/tr.length));
		this->mRotation = tr.getRotation((this->mLengthRemaining/tr.length), true);
	}
	else {
		//to second
		this->mPosition = tr.getPosition(1.0-(this->mLengthRemaining/tr.length));
		this->mRotation = tr.getRotation(1.0-(this->mLengthRemaining/tr.length), false);
	}

	return this->mPosition;
}
void BogieMovement::applyMove(Map& aMap, Line& aLine) noexcept {
	glm::vec3 rv = glm::rotate(this->mBogieOffset, glm::radians(this->mRotation.y), glm::vec3(0.0, 1.0, 0.0));
	rv = glm::rotate(rv, glm::radians(this->mRotation.z), glm::vec3(0.0, 0.0, 1.0));

	this->mBogie->getTransform().setPosition(this->mPosition+rv);
	this->mBogie->getTransform().setRotation(this->mRotation);
	this->mShaft1->getTransform().setPosition(this->mPosition+rv);
	this->mShaft1->getTransform().setRotation(this->mRotation);
	this->mShaft2->getTransform().setPosition(this->mPosition+rv);
	this->mShaft2->getTransform().setRotation(this->mRotation);
}

bool BogieMovement::isValid() const noexcept {
	bool valid = this->mBogie && this->mShaft1 && this->mShaft2;
	return valid;
}
void BogieMovement::validate() const noexcept {
	if(!this->isValid()) {
		std::cerr << LogLevel::ERROR << "Bogie movement handler's models not set!\n" << LogLevel::RESET;
	}
}

BogieMovement::~BogieMovement() noexcept {}

Vehicle::Vehicle(Map& aMap, Line& aLine, VehicleInformation& aInfo, Model* aModel) noexcept {
	if(!aInfo.validate()) {
		std::cerr << LogLevel::ERROR << "Vehicle information invalid!\n" << LogLevel::RESET;
		return;
	}

	this->mModel = nullptr;
	this->mInfo = aInfo;
	this->init(aMap, aLine, aModel);
}

/*
 * CONFIG VALUES
 * --
 * NAME=VALUE
 * NAME=VALUE,VALUE;
 */

constexpr char SEPARATOR_LOW = ',';
constexpr char SEPARATOR_HIGH = ';';

static void loadVector3(const std::string_view aInput, glm::vec3& aVector, const std::string_view aName) {
	std::vector<std::string> vector;
	splitString(aInput.data(), SEPARATOR_LOW, vector);
	if(vector.size() != 3) {
		std::cerr << LogLevel::ERROR << "Vector " << aName << " doesn't contain 3 values! (" << vector.size() << " values found)\n" << LogLevel::RESET;
		return;
	}
	aVector = glm::vec3(std::stof(vector[0]), std::stof(vector[1]), std::stof(vector[2]));
}

static void loadVector3r(const std::string_view aInput, glm::vec3& aVector, float& aFloat, const std::string_view aName) {
	std::vector<std::string> vector;
	splitString(aInput.data(), SEPARATOR_LOW, vector);
	if(vector.size() != 4) {
		std::cerr << LogLevel::ERROR << "Vector " << aName << " doesn't contain 3 values + float! (" << vector.size() << " values found)\n" << LogLevel::RESET;
		return;
	}
	aVector = glm::vec3(std::stof(vector[0]), std::stof(vector[1]), std::stof(vector[2]));
	aFloat = std::stof(vector[3]);
}

Vehicle::Vehicle(const std::string_view aConfigFilename) noexcept {
	std::ifstream file;
	file.open(aConfigFilename.data(), std::ios::in); //text mode
	if(!file.is_open()) {
		std::cerr << LogLevel::ERROR << "File " << aConfigFilename << " could not be opened!\n" << LogLevel::RESET;
		return;
	}

	this->mCabinData.data.resize(VehicleCabinTriggerData::CONTROL_AMOUNT);

	std::string buffer;
	while(std::getline(file, buffer)) {
		if(buffer.empty()) continue;
		if(buffer[0] == '#') continue; //comment

		std::string name = buffer.substr(0, buffer.find('='));
		std::string value = buffer.substr(buffer.find('=')+1);

		std::cout << "Vehicle config: " << value << " is " << name << '\n';

		if(name == "MODEL") this->mInfo.modelFile = value; //model loading/setting separate
		else if(name == "MASS") this->mInfo.mass = std::stof(value);
		else if(name == "LENGTH") this->mInfo.length = std::stof(value);
		else if(name == "WIDTH") this->mInfo.width = std::stof(value);
		else if(name == "HEIGHT") this->mInfo.height = std::stof(value);
		else if(name == "SEATS") this->mInfo.seats = std::stoull(value);
		else if(name == "STANDING") this->mInfo.standing = std::stoull(value);
		else if(name == "MOTORS") this->mInfo.motorAmount = std::stof(value);
		else if(name == "POWER") this->mPhysicsData.power = std::stof(value);
		else if(name == "SPEED") this->mPhysicsData.maxSpeed = std::stof(value);
		else if(name == "MOTORRPM") this->mInfo.motorRPM = std::stof(value);
		else if(name == "GEARRATIO") this->mInfo.gearRatio = std::stof(value);
		else if(name == "WHEELDIAMETER") this->mInfo.wheelDiameter = std::stof(value);
		else if(name == "AEROCOEF") this->mInfo.aerodynamicCoef = std::stof(value);
		else if(name == "BOGIENAMES") {
			std::vector<std::string> names;
			splitString(value, SEPARATOR_LOW, names);
			this->mInfo.bogieNames = std::move(names);
		}
		else if(name == "BOGIEWHEELS") {
			std::vector<std::string> suffixes;
			splitString(value, SEPARATOR_LOW, suffixes);
			if(suffixes.size() != 2) {
				std::cerr << LogLevel::ERROR << "More than 2 axle suffixes present - tram bogie cannot have more than 2 axles! (" <<suffixes.size() << " found)\n" << LogLevel::RESET;
				return;
			}
			this->mInfo.bogieShaftSuffixes = std::move(suffixes);
		}
		else if(name == "BOGIEMESHES") {
			if(value.size() == 0) {
				std::cerr << LogLevel::WARNING << "No meshes set, using default.\n" << LogLevel::RESET;
				continue; //dont bother
			}

			std::vector<std::string> meshNameLists;
			splitString(value, SEPARATOR_HIGH, meshNameLists);

			for(uint64_t i = 0; i < meshNameLists.size(); i++) {
				std::vector<std::string> meshNames;
				splitString(meshNameLists[i], SEPARATOR_LOW, meshNames);
				this->mInfo.meshNames.push_back(meshNames);
			}
		}
		else if(name == "BOGIEOFFSETS") {
			std::vector<std::string> offsets;
			splitString(value, SEPARATOR_HIGH, offsets);
			for(uint64_t i = 0; i < offsets.size(); i++) {
				std::vector<std::string> vector;
				splitString(offsets[i], SEPARATOR_LOW, vector);
				if(vector.size() != 2) {
					std::cerr << LogLevel::ERROR << "Vector doesn't contain 2 values: add only values for X and Z axes! (" << vector.size() << " values found)\n" << LogLevel::RESET;
					return;
				}
				this->mInfo.bogieCenterOffset.push_back(glm::vec3(std::stof(vector[0]), 0.0f, std::stof(vector[1])));
			}
		}
		else if(name == "BOGIETROFFSETS") {
			std::vector<std::string> offsets;
			splitString(value, SEPARATOR_LOW, offsets);
			for(uint64_t i = 0; i < offsets.size(); i++) {
				this->mInfo.bogieTrackOffset.push_back(std::stof(offsets[i]));
			}
		}
		else if(name == "LIVERYMAT") this->mInfo.variantMaterial = value;
		else if(name == "LIVERY") {
			std::vector<std::string> values;
			splitString(value, SEPARATOR_LOW, values);
			this->mInfo.variants.push_back(std::make_pair(values[0], values[1]));
		}
		else if(name == "CAMERAOFFSET") loadVector3(value, this->mInfo.cameraOffset, "CAMERAOFFSET");

		//TODO controls setup

		//starter
		else if(name == "STARTEROFFSET") loadVector3(value, this->mCabinData.data[VehicleCabinTriggerData::STARTER].offset, "STARTEROFFSET");
		else if(name == "STARTERSIZE") loadVector3r(value,
			this->mCabinData.data[VehicleCabinTriggerData::STARTER].size,
			this->mCabinData.data[VehicleCabinTriggerData::STARTER].rotation, "STARTERSIZE");

		//sander
		else if(name == "SANDEROFFSET") loadVector3(value, this->mCabinData.data[VehicleCabinTriggerData::SANDER].offset, "SANDEROFFSET");
		else if(name == "SANDERSIZE") loadVector3r(value,
			this->mCabinData.data[VehicleCabinTriggerData::SANDER].size,
			this->mCabinData.data[VehicleCabinTriggerData::SANDER].rotation, "SANDERSIZE");

		//stop announcement
		else if(name == "STOPOFFSET") loadVector3(value, this->mCabinData.data[VehicleCabinTriggerData::STOP].offset, "STOPOFFSET");
		else if(name == "STOPSIZE") loadVector3r(value,
			this->mCabinData.data[VehicleCabinTriggerData::STOP].size,
			this->mCabinData.data[VehicleCabinTriggerData::STOP].rotation, "STOPSIZE");

		//TODO others

		//TODO config sounds - load into vehicle itself so multiple vehicle can play at once
		else if(name == "SOUNDOPENDOOR") {}
		else if(name == "SOUNDCLOSEDOOR") {}
		else if(name == "SOUNDOPENCABIN") {}
		else if(name == "SOUNDCLOSECABIN") {}
		else if(name == "SOUNDSTART") {}
		else if(name == "SOUNDRIDE1") {}
		else if(name == "SOUNDRRIDE1") {}
		else if(name == "SOUNDRIDE2") {}
		else if(name == "SOUNDRRIDE2") {}
		else if(name == "SOUNDRIDE3") {}
		else if(name == "SOUNDRRIDE3") {}
		else if(name == "SOUNDBRAKE") {}
		else if(name == "SOUNDSWITCH") {}
		else if(name == "SOUNDINFO") {}

		//future config values go here TODO

		else {
			std::cerr << LogLevel::ERROR << "Unknown configuration value "+name+" - check the spelling and try again.\n" << LogLevel::RESET;
			return;
		}
	}

	file.close();

	if(!this->mInfo.validate()) {
		std::cerr << LogLevel::ERROR << "Config invalid!\n" << LogLevel::RESET;
		return;
	}

	this->mInfo.areaFront = this->mInfo.height*this->mInfo.width;

	std::cout << "Vehicle configration loaded!\n";
}

Vehicle::Vehicle(Vehicle&& aOther) noexcept {
	this->mModel = std::move(aOther.mModel);
	aOther.mModel = nullptr;
	this->mBogieMeshes = std::move(aOther.mBogieMeshes);
	this->mInfo = std::move(aOther.mInfo);
	this->mPhysicsData = std::move(aOther.mPhysicsData);
	this->mBogies = std::move(aOther.mBogies);
	this->mControlData = std::move(aOther.mControlData);
	this->mCabinData = std::move(aOther.mCabinData);
	this->mTriggerDrawer = std::move(aOther.mTriggerDrawer);
	this->mTriggers = std::move(aOther.mTriggers);
	this->mCameraLocation = std::move(aOther.mCameraLocation);
	this->mCameraRotation = std::move(aOther.mCameraRotation);
}
Vehicle& Vehicle::operator=(Vehicle&& aOther) noexcept {
	this->mModel = std::move(aOther.mModel);
	aOther.mModel = nullptr;
	this->mBogieMeshes = std::move(aOther.mBogieMeshes);
	this->mInfo = std::move(aOther.mInfo);
	this->mPhysicsData = std::move(aOther.mPhysicsData);
	this->mBogies = std::move(aOther.mBogies);
	this->mControlData = std::move(aOther.mControlData);
	this->mCabinData = std::move(aOther.mCabinData);
	this->mTriggerDrawer = std::move(aOther.mTriggerDrawer);
	this->mTriggers = std::move(aOther.mTriggers);
	this->mCameraLocation = std::move(aOther.mCameraLocation);
	this->mCameraRotation = std::move(aOther.mCameraRotation);
	return *this;
}

std::string Vehicle::getConfigModelFilename() const noexcept {
	return this->mInfo.modelFile;
}
void Vehicle::init(Map& aMap, Line& aLine, Model* aModel) noexcept {
	this->mModel = aModel;

	for(uint64_t i = 0; i < this->mInfo.bogieNames.size(); i++) {
		this->mBogies.emplace_back();
		this->mBogies[i].init(aMap, aLine, this->mInfo.bogieTrackOffset[i]);
		this->mBogies[i].setData(*this->mModel, this->mInfo.bogieNames[i], this->mInfo.bogieShaftSuffixes[0], this->mInfo.bogieShaftSuffixes[1], this->mInfo.bogieCenterOffset[i]);
		this->mBogies[i].validate();

		//add meshes which are to be update
		if(i != 0 && !this->mInfo.meshNames.empty()) {
			this->mBogieMeshes.emplace_back();
			for(uint64_t j = 0; j < this->mInfo.meshNames[i-1].size(); j++) {
				Mesh* ptr = this->mModel->getMesh(this->mInfo.meshNames[i-1][j]);
				if(ptr) {
					this->mBogieMeshes.back().push_back(ptr);
				}
				else {
					std::cerr << LogLevel::WARNING << "Invalid mesh with name " << this->mInfo.meshNames[i-1][j] << " passed to vehicle.\n" << LogLevel::RESET;
				}
			}
		}
	}

	for(uint64_t i = 0; i < this->mInfo.variants.size(); i++) {
		//first in pair is name, second in pair is filepath
		this->mModel->addVariant(this->mInfo.variantMaterial, this->mInfo.variants[i].second, this->mInfo.variants[i].first);
	}

	//add trigger boxes TODO more
	for(VehicleCabinTriggerDataEntry& v : this->mCabinData.data) {
		this->mTriggers.emplace_back(v.offset, v.size, v.rotation);
		this->mTriggers.back().setColor(glm::vec4(0.0, 1.0, 0.0, 0.5));
	}

	//add to drawer
	for(BoxTrigger& bt : this->mTriggers) mTriggerDrawer.add(bt);
}

//TODO reset function

void Vehicle::update(Map& aMap, Line& aLine) noexcept {
	std::vector<glm::vec3> positions;
	bool leading = true;
	for(BogieMovement& b : this->mBogies) {
		if(leading) {
			//if leading bogie on last station and not at our station track - exit
			if(aLine.isStationLast() && !aMap.isTrackStation(b.mTrackId)) return;
		}
		auto a = b.move(aMap, aLine, this->mPhysicsData.speed, leading);
		if(!a.has_value()) {
			std::cout << "Dropping bogie!\n";
			//if we are at last and want to move further - exit
			return; //station last, no value
		}
		positions.push_back(a.value());
		leading = false; //only for first bogie
	}

	//calculate first transform
	glm::vec3 avg = Math::getAverageOfVectors(positions[1], positions[0]);
	glm::vec3 dif = positions[0] - positions[1];
	float rotationHorizontal = Math::getRotationOfVector2DY(glm::vec2(dif.x, dif.z));
	float rotationVertical = Math::getRotationOfVector2DX(glm::vec2(std::sqrt(std::pow(dif.x,2.0)+std::pow(dif.z,2.0)), dif.y));

	//apply to camera
	this->mCameraLocation = avg+glm::rotate(glm::rotate(this->mInfo.cameraOffset, glm::radians(rotationHorizontal), glm::vec3(0, 1, 0)), rotationVertical, glm::vec3(1, 0, 0));
	this->mCameraRotation = glm::vec3(rotationVertical, -rotationHorizontal, 0.0);

	//apply to controls
	for(uint64_t i = 0; i < this->mTriggers.size(); i++) {
		BoxTrigger& bt = this->mTriggers[i];
		VehicleCabinTriggerDataEntry& v = this->mCabinData.data[i];

		bt.setCenterPoint(avg+glm::rotate(glm::rotate(v.offset, glm::radians(rotationHorizontal), glm::vec3(0, 1, 0)), rotationVertical, glm::vec3(1, 0, 0)));
		bt.setRotation(rotationHorizontal);
	}

	//move meshes

	//default - apply first pair to all
	if(this->mBogieMeshes.empty()) {
		this->mModel->getGlobalTransform().setPosition(avg);
		//due to some rotation crap we need to add 180 degress to flip AND camera TODO improve
		this->mModel->getGlobalTransform().setRotation(glm::vec3(rotationVertical, rotationHorizontal+180, 0));
		this->mModel->refreshTransforms();
	}
	else {
		//else not needed
		for(std::vector<Mesh*> v : this->mBogieMeshes) {
			for(uint64_t j = 0; j < v.size(); j++) {
				glm::vec3 avg = Math::getAverageOfVectors(positions[j+1], positions[j]);
				//switch Z and Y - Z axis in simulator is y field, height in sim is z field
				glm::vec3 dif = positions[j] - positions[j+1];

				float rotationHorizontal = Math::getRotationOfVector2DY(glm::vec2(dif.x, dif.z));
				float rotationVertical = Math::getRotationOfVector2DX(glm::vec2(std::sqrt(std::pow(dif.x,2.0)+std::pow(dif.z,2.0)), dif.y));

				this->mModel->getGlobalTransform().setPosition(avg);
				//same here
				this->mModel->getGlobalTransform().setRotation(glm::vec3(rotationVertical, rotationHorizontal+180, 0));
				this->mModel->refreshTransforms();
			}
		}
	}

	//apply bogie transform after global
	for(BogieMovement& m : this->mBogies) {
		m.applyMove(aMap, aLine);
	}
}

void Vehicle::physicsUpdate(const uint16_t aWeather, const float aPhysicsUpdateFreq) noexcept {
	//calculate forces

	//power in KW
	this->mPhysicsData.fceFront = Physics::forceFromPower(
		this->mPhysicsData.power*this->mInfo.motorAmount*1000, this->mControlData.throttle,
		this->mInfo.motorRPM, this->mInfo.gearRatio, this->mPhysicsData.physicsSpeed, this->mInfo.wheelDiameter
	);
	this->mPhysicsData.fceAerodynamic = Physics::forceAerodynamic(this->mPhysicsData.physicsSpeed, this->mInfo.areaFront, this->mInfo.aerodynamicCoef);
	this->mPhysicsData.fceGravity = Physics::forceGravity(this->mInfo.mass);
	this->mPhysicsData.fceNormal = Physics::forceNormal(this->mPhysicsData.fceGravity, this->mBogies[0].mRotation.x);

	//friction
	float frictionCoef = Physics::getWeatherFrictionCoeff((WeatherCondition)aWeather, this->mPhysicsData.physicsSpeed);
	this->mPhysicsData.fceFriction = Physics::forceFriction(this->mPhysicsData.fceNormal, frictionCoef);
	float appliedFrictionForce = 0.0;
	if(this->mControlData.brakeEmergency) appliedFrictionForce = this->mPhysicsData.fceFriction;

	//rolling resistance
	this->mPhysicsData.fceResistance = Physics::forceRollingResistance(this->mPhysicsData.fceNormal);

	this->mPhysicsData.fceTurn = Physics::forceTurn(
		this->mInfo.mass, this->mPhysicsData.physicsSpeed,
		Math::getCurveRadius(
			Math::swizzleXZ(this->mBogies[0].mPosition), this->mBogies[0].mRotation.y,
			Math::swizzleXZ(this->mBogies[1].mPosition), this->mBogies[1].mRotation.y
		)
	);

	//vertical movement - Nadal and so on
	this->mPhysicsData.lvr = this->mPhysicsData.fceTurn/this->mPhysicsData.fceNormal;
	this->mPhysicsData.nadal = Physics::nadalLimit(frictionCoef);
	this->mPhysicsData.fceVertical = Physics::verticalForce(this->mPhysicsData.physicsSpeed, this->mPhysicsData.fceTurn);
	this->mPhysicsData.verticalAcceleration = Physics::accelerationFromForce(this->mPhysicsData.fceVertical - this->mPhysicsData.fceGravity, this->mInfo.mass);
	//get extra speed per second (m/s), divide by physics update frequency
	this->mPhysicsData.verticalSpeed += Physics::perFrameSpeedFromAcceleration(this->mPhysicsData.verticalAcceleration, 1.0/aPhysicsUpdateFreq);
	this->mPhysicsData.verticalDistanceTravelled += this->mPhysicsData.verticalSpeed/aPhysicsUpdateFreq;

	//clamp to zero
	this->mPhysicsData.verticalDistanceTravelled = std::max(this->mPhysicsData.verticalDistanceTravelled, 0.0f);
	if(this->mPhysicsData.verticalDistanceTravelled == 0.0) {
		this->mPhysicsData.verticalSpeed = 0.0;
		this->mPhysicsData.verticalDistanceTravelled = 0.0;
	}

	if(this->mPhysicsData.verticalDistanceTravelled > TRACK_HEIGHT) {
		std::cerr << LogLevel::SUCCESS << "Vehicle derailed!\n" << LogLevel::RESET;
		std::exit(0);
	}

	float maxResistance = Physics::maxResistanceForce(this->mPhysicsData.physicsSpeed, this->mInfo.mass, 1.0/aPhysicsUpdateFreq);
	this->mPhysicsData.fceResult = Physics::resultingForce(
		this->mPhysicsData.fceFront,
		appliedFrictionForce + std::abs(this->mPhysicsData.fceResistance) + std::abs(this->mPhysicsData.fceAerodynamic),
		maxResistance, this->mPhysicsData.physicsSpeed
	);

	if(std::abs(this->mPhysicsData.fceFront) > std::abs(this->mPhysicsData.fceFriction)) {
		//slip! - dont apply anything
		std::cout << "SLIP!\n";
	}
	else {
		//horizontal speed
		this->mPhysicsData.acceleration = Physics::accelerationFromForce(this->mPhysicsData.fceResult, this->mInfo.mass);
		//get extra speed per second (m/s), divide by physics update frequency
		this->mPhysicsData.physicsSpeed += Physics::perFrameSpeedFromAcceleration(this->mPhysicsData.acceleration, 1.0/aPhysicsUpdateFreq);
		this->mPhysicsData.speed = this->mPhysicsData.physicsSpeed/aPhysicsUpdateFreq;
	}
}
bool Vehicle::setSpeed(const float aSpeed) noexcept {
	this->mPhysicsData.speed = aSpeed;
	if(aSpeed >= this->mPhysicsData.maxSpeed) return false;
	return true;
}

VehicleControlData* Vehicle::getVehicleControlData() noexcept {
	return &this->mControlData;
}
VehiclePhysicsData* Vehicle::getVehiclePhysicsData() noexcept {
	return &this->mPhysicsData;
}
glm::vec3 Vehicle::getCameraPosition() const noexcept {
	return this->mCameraLocation;
}
glm::vec3 Vehicle::getCameraRotation() const noexcept {
	return this->mCameraRotation;
}

BoxTriggerDrawer& Vehicle::getControlsDraw() noexcept {
	return this->mTriggerDrawer;
}
std::vector<BoxTrigger>& Vehicle::getTriggers() noexcept {
	return this->mTriggers;
}

Vehicle::~Vehicle() noexcept {}

void UI::drawPhysicsInfoWindow(Vehicle& aVehicle) noexcept {
	ImGui::Begin("Vehicle physics data");

	ImGui::Text("Speed: %f m/s", aVehicle.mPhysicsData.physicsSpeed);
	ImGui::Text("Total energy used: %f J", aVehicle.mPhysicsData.energyUsed);

	ImGui::Text("Resulting force: %f N", aVehicle.mPhysicsData.fceResult);
	ImGui::Text("Movement force: %f N", aVehicle.mPhysicsData.fceFront);
	ImGui::Text("Gravitational force (vertical): %f N", aVehicle.mPhysicsData.fceGravity);
	ImGui::Text("Rolling resistance: %f N", aVehicle.mPhysicsData.fceResistance);
	ImGui::Text("Friction force: %f N", aVehicle.mPhysicsData.fceFriction);
	ImGui::Text("Aerodynamic resistance: %f N", aVehicle.mPhysicsData.fceAerodynamic);
	ImGui::Text("Cetripetal force: %f N", aVehicle.mPhysicsData.fceTurn);
	ImGui::Text("Vertical force: %f N", aVehicle.mPhysicsData.fceVertical);
	ImGui::Text("L/V ratio: %f ", aVehicle.mPhysicsData.lvr);
	ImGui::Text("L/V, Nadal limit: %f ", aVehicle.mPhysicsData.nadal);
	ImGui::Text("Wheel climb dst: %f ", aVehicle.mPhysicsData.verticalDistanceTravelled);
	ImGui::Text(""); //empty line
	ImGui::Text("Voltage: %f V", aVehicle.mPhysicsData.voltage);
	ImGui::Text(""); //empty line
	ImGui::Text("Friction cf.: %f", aVehicle.mPhysicsData.frictionCoef);

	ImGui::End();
}
