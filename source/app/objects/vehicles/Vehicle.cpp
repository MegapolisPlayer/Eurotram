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

glm::vec2 BogieMovement::move(Map& aMap, Line& aLine, const float aAmount, const bool aUpdateStation) noexcept {
	if(aLine.isStationLast()) return glm::vec2(0.0);

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

	//get position of bogies

	Track& tr = aMap.getTrackById(this->mTrackId);
	glm::vec3 v, r;
	if(
		(int32_t)this->mNextNodeId.second == tr.first &&
		(
			((tr.flags & TRACK_FLAG_FIRST_SWITCH) && this->mNextNodeId.first == 's') ||
			(!(tr.flags & TRACK_FLAG_FIRST_SWITCH) && this->mNextNodeId.first == 'n')
		)
	) {
		//to first
		v = tr.getPosition((this->mLengthRemaining/tr.length));
		r = tr.getRotation((this->mLengthRemaining/tr.length), true);
	}
	else {
		//to second
		v = tr.getPosition(1.0-(this->mLengthRemaining/tr.length));
		r = tr.getRotation(1.0-(this->mLengthRemaining/tr.length), false);
	}

	glm::vec3 rv = glm::rotate(this->mBogieOffset, glm::radians(r.y), glm::vec3(0.0, 1.0, 0.0));
	rv = glm::rotate(rv, glm::radians(r.z), glm::vec3(0.0, 0.0, 1.0));
	this->mBogie->getTransform().setPosition(v+rv);
	this->mBogie->getTransform().setRotation(r);
	this->mShaft1->getTransform().setPosition(v+rv);
	this->mShaft1->getTransform().setRotation(r);
	this->mShaft2->getTransform().setPosition(v+rv);
	this->mShaft2->getTransform().setRotation(r);

	return v;
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

	for(uint64_t i = 0; i < aInfo.bogieNames.size(); i++) {
		this->mBogies.emplace_back();
		this->mBogies[i].init(aMap, aLine, aInfo.bogieTrackOffset[i]);
		this->mBogies[i].setData(*aInfo.model, aInfo.bogieNames[i], aInfo.bogieShaftSuffixes[0], aInfo.bogieShaftSuffixes[1], aInfo.bogieCenterOffset[i]);
		this->mBogies[i].validate();
	}
}

void Vehicle::update(Map& aMap, Line& aLine) noexcept {
	bool leading = true;
	for(BogieMovement& b : this->mBogies) {
		b.move(aMap, aLine, this->mPhysicsData.speed, leading);
		leading = false; //only for first bogie
	}
}

VehiclePhysicsData* Vehicle::getVehiclePhysicsData() noexcept {
	return &this->mPhysicsData;
}

Vehicle::~Vehicle() noexcept {

}
