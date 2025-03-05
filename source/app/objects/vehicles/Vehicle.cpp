#include "Vehicle.hpp"

BogieMovement::BogieMovement() noexcept
	: mBogie(nullptr), mShaft1(nullptr), mShaft2(nullptr), mLengthRemaining(0.0), mTrackId(0), mSwitchCount(0) {}
void BogieMovement::setData(
	Mesh* aBogieName,
	Mesh* aBogieShaftSuffix1,
	Mesh* aBogieShaftSuffix2,
	const glm::vec3& aBogieOffset
) noexcept {
	this->mBogie = aBogieName;
	this->mShaft1 = aBogieShaftSuffix1;
	this->mShaft2 = aBogieShaftSuffix2;
	this->mBogieOffset = aBogieOffset;
}
glm::vec2 BogieMovement::move(Map* aMap, Line* aLine, const float aAmount, const bool aUpdateStation) noexcept {
	this->mLengthRemaining -= aAmount;

	//update track

	if(this->mLengthRemaining <= 0) {
		if(this->mSwitchCount >= aLine->getSwitchesToNextStop().size()) {
			//we already passed last switch - no more setting of value (if reach switch - game over)
			this->mTrackId = aMap->getNextTrack(this->mTrackId, this->mNextNodeId, LineData::SwitchDirection::NO_SET);
		}
		else {
			this->mTrackId = aMap->getNextTrack(this->mTrackId, this->mNextNodeId, aLine->getSwitchesToNextStop()[this->mSwitchCount].direction);
		}
		if(this->mNextNodeId.first == 's') {
			this->mSwitchCount++;
		}
		this->mNextNodeId = aMap->getOtherTrackPoint(this->mTrackId, this->mNextNodeId);
		if(aMap->isTrackStation(this->mTrackId)) {
			this->mSwitchCount = 0;
			if(aUpdateStation) {
				aLine->nextStation();
			}
		}
		this->mLengthRemaining += aMap->getTrackById(this->mTrackId).length;
	}

	//get position of bogies

	Track& tr = aMap->getTrackById(this->mTrackId);
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

	this->mBogie->getTransform().setPosition(v+this->mBogieOffset);
	this->mBogie->getTransform().setRotation(r);
	this->mShaft1->getTransform().setPosition(v+this->mBogieOffset);
	this->mShaft1->getTransform().setRotation(r);
	this->mShaft2->getTransform().setPosition(v+this->mBogieOffset);
	this->mShaft2->getTransform().setRotation(r);

	return v;
}
BogieMovement::~BogieMovement() noexcept {}

Vehicle::Vehicle() noexcept {

}

void Vehicle::movementUpdate() noexcept {
	bool leading = true;
	for(BogieMovement& b : this->mBogies) {
		b.move(this->mMap, this->mLine, this->mData.speed, leading);
		leading = false; //only for first bogie
	}
}
void Vehicle::physicsUpdate() noexcept {

}

Vehicle::~Vehicle() noexcept {

}
