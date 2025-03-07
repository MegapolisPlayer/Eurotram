#include "Vehicle.hpp"

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

std::optional<glm::vec3> BogieMovement::move(Map& aMap, Line& aLine, const float aAmount, const bool aUpdateStation) noexcept {
	if(aLine.isStationLast()) return {};

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
		if(aLine.isStationLast()) {
			return {};
		}
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

Vehicle::Vehicle(Map& aMap, Line& aLine, VehicleInformation& aInfo) noexcept {
	//TODO add error info and more validation
	assert(aInfo.bogieShaftSuffixes.size() == 2);
	assert(aInfo.bogieNames.size() == aInfo.bogieTrackOffset.size());
	assert(aInfo.meshNames.size() == aInfo.bogieNames.size()-1 || aInfo.meshNames.empty());

	this->mModel = aInfo.model;

	for(uint64_t i = 0; i < aInfo.bogieNames.size(); i++) {
		this->mBogies.emplace_back();
		this->mBogies[i].init(aMap, aLine, aInfo.bogieTrackOffset[i]);
		this->mBogies[i].setData(*aInfo.model, aInfo.bogieNames[i], aInfo.bogieShaftSuffixes[0], aInfo.bogieShaftSuffixes[1], aInfo.bogieCenterOffset[i]);
		this->mBogies[i].validate();

		//add meshes which are to be update
		if(i != 0 && !aInfo.meshNames.empty()) {
			this->mBogieMeshes.emplace_back();
			for(uint64_t j = 0; j < aInfo.meshNames[i-1].size(); j++) {
				Mesh* ptr = aInfo.model->getMesh(aInfo.meshNames[i-1][j]);
				if(ptr) {
					this->mBogieMeshes.back().push_back(ptr);
				}
				else {
					std::cerr << LogLevel::WARNING << "Invalid mesh with name " << aInfo.meshNames[i-1][j] << " passed to vehicle.\n" << LogLevel::RESET;
				}
			}
		}
	}
}

void Vehicle::update(Map& aMap, Line& aLine) noexcept {
	std::vector<glm::vec3> positions;
	bool leading = true;
	for(BogieMovement& b : this->mBogies) {
		auto a = b.move(aMap, aLine, this->mPhysicsData.speed, leading);
		if(!a.has_value()) {
			return; //station last, no value
		}
		positions.push_back(a.value());
		leading = false; //only for first bogie
	}

	//if we reached last - exit
	if(aLine.isStationLast()) return;

	//move meshes

	//default - apply first pair to all
	if(this->mBogieMeshes.empty()) {
		glm::vec3 avg = Math::getAverageOfVectors(positions[1], positions[0]);
		glm::vec3 dif = glm::vec3((positions[0] - positions[1]).x, (positions[0] - positions[1]).y, (positions[0] - positions[1]).z);

		float rotationHorizontal = Math::getRotationOfVector2DY(glm::vec2(dif.x, dif.z));
		float rotationVertical = Math::getRotationOfVector2DX(glm::vec2(std::sqrt(std::pow(dif.x,2.0)+std::pow(dif.z,2.0)), dif.y));

		this->mModel->getGlobalTransform().setPosition(avg);
		//due to some rotation crap we need to add 180 degress to flip TODO improve
		this->mModel->getGlobalTransform().setRotation(glm::vec3(rotationVertical, rotationHorizontal+180, 0));
		this->mModel->refreshTransforms();
	}
	else {
		//else not needed
		for(std::vector<Mesh*> v : this->mBogieMeshes) {
			for(uint64_t j = 0; j < v.size(); j++) {
				glm::vec3 avg = Math::getAverageOfVectors(positions[j+1], positions[j]);
				//switch Z and Y - Z axis in simulator is y field, height in sim is z field
				glm::vec3 dif = glm::vec3((positions[j] - positions[j+1]).x, (positions[j] - positions[j+1]).y, (positions[j] - positions[j+1]).z);

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

VehiclePhysicsData* Vehicle::getVehiclePhysicsData() noexcept {
	return &this->mPhysicsData;
}

Vehicle::~Vehicle() noexcept {}
