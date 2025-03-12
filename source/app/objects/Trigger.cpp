#include "Trigger.hpp"

//as is usual: simple 1x1x1 cube in VBO, then we scale and translate

static std::vector<float> BT_VERTICES_BASE = {
	-0.5, -0.5f, 0.5f,
	0.5, -0.5f, 0.5f,
	0.5f,  0.5f, 0.5f,
	-0.5f,  0.5f, 0.5f,

	-0.5, -0.5f, -0.5f,
	0.5, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
};

//all in CCW order so backface culling works!
static GLuint BT_INDICES[] = {
	//front, back
	0, 1, 2, 2, 3, 0,
	5, 4, 7, 7, 6, 5,

	//bottom, top
	0, 4, 5, 5, 1, 0,
	3, 2, 6, 6, 7, 3,

	//right, left
	1, 5, 6, 6, 2, 1,
	4, 0, 3, 3, 7, 4,
};

BoxTriggerDrawer::BoxTriggerDrawer() noexcept : mVBO(BT_VERTICES_BASE.data(), 8, 3), mIBO(BT_INDICES, 36),
	mTransforms(nullptr, 0),  mColors(nullptr, 0), mRecalculateData(true) {
	this->mVBO.enableAttribute(&this->mVAO, 3); //pos
}

uint64_t BoxTriggerDrawer::add(BoxTrigger& aBT) noexcept {
	this->mTriggerList.push_back(&aBT);
	this->mRecalculateData = true;
	return this->mTransformsData.size()-1;
}
void BoxTriggerDrawer::remove(const uint64_t aId) noexcept {
	this->mTriggerList.erase(this->mTriggerList.begin()+aId);
	this->mRecalculateData = true;
}
void BoxTriggerDrawer::draw(const uint64_t aTransformLocation, const uint64_t aColorLocation) noexcept {
	if(this->mRecalculateData) {
		std::vector<glm::vec4> colorData;
		this->mTransformsData.resize(this->mTriggerList.size());

		for(uint64_t i = 0; i < this->mTriggerList.size(); i++) {
			BoxTrigger* ptr = this->mTriggerList[i];
			if(ptr->mContainsNewData) {
				if(!ptr->mVisible) {
					this->mTransformsData.emplace_back(glm::mat4(0.0)); //null matrix
				}
				else {
					this->mTransformsData[i] = glm::translate(glm::mat4(1.0), ptr->mCenterPoint);
					this->mTransformsData[i] = glm::rotate(this->mTransformsData[i], ptr->mRotation, glm::vec3(0, 1, 0));
					this->mTransformsData[i] = glm::scale(this->mTransformsData[i], ptr->mSize);
				}
				ptr->mContainsNewData = false;
			}
			colorData.emplace_back(ptr->mColor);
		}

		this->mTransforms.setNewData(this->mTransformsData.data(), this->mTransformsData.size()*sizeof(glm::mat4));
		this->mColors.setNewData(colorData.data(), colorData.size()*sizeof(glm::vec4));
		this->mRecalculateData = false;
	}

	this->mVAO.bind();
	this->mVBO.bind();
	this->mIBO.bind();
	this->mTransforms.bind(aTransformLocation);
	this->mColors.bind(aColorLocation);
	this->mIBO.drawInstanced(this->mTransformsData.size());
}

BoxTriggerDrawer::~BoxTriggerDrawer() noexcept {

}

BoxTrigger::BoxTrigger(const glm::vec3& aCenter, const glm::vec3& aSize, const float aRotation) noexcept
	: mCenterPoint(aCenter), mSize(aSize), mRotation(aRotation), mEnabled(true), mVisible(true), mColor(1.0f), mContainsNewData(true) {}

//separated axis theorem
//all of our polygons have 4 edges
//rotation only around Y axis - 2D collision enough
//source https://dyn4j.org/2010/01/sat/

//utils for collision
struct Projection {
	float min, max;
};

Projection projectShape(const glm::vec2& aAxis, const glm::vec2& a1, const glm::vec2& a2, const glm::vec2& a3, const glm::vec2& a4) {
	double min = glm::dot(aAxis, a1);
	double max = min;

	//vert 2
	double p = glm::dot(aAxis, a2);
	if(p < min) min = p;
	else if(p > max) max = p;

	//vert 3
	p = glm::dot(aAxis, a3);
	if(p < min) min = p;
	else if(p > max) max = p;

	//vert 4
	p = glm::dot(aAxis, a4);
	if(p < min) min = p;
	else if(p > max) max = p;

	return Projection(min, max);
}

bool projectionOverlapping(const Projection& a1, const Projection& a2) {
	//std::cout << "<" << a1.min << ',' << a1.max << ">&<" << a2.min << ',' << a2.max << "> -> " << (a1.min <= a2.max && a2.min <= a1.max) << '\n'; //DEBUG
	return a1.min <= a2.max && a2.min <= a1.max;
}

bool BoxTrigger::collision(BoxTrigger& aOther) {
	if(!this->mEnabled) return false;
	if(!aOther.mEnabled) return false;

	//get AND ROTATE vertices
	glm::mat4 rotationA = glm::rotate(glm::mat4(1.0f), glm::radians(this->mRotation), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationB = glm::rotate(glm::mat4(1.0f), glm::radians(aOther.mRotation), glm::vec3(0.0f, 1.0f, 0.0f));

	//for some weird reason building transform matrix with center point doesnt work
	//this does tho
	glm::vec2 vertexA1 = Math::swizzleXZ(this->mCenterPoint + glm::vec3(rotationA * glm::vec4(-this->mSize.x/2.0, 0.0, -this->mSize.z/2.0, 0.0)));
	glm::vec2 vertexA2 = Math::swizzleXZ(this->mCenterPoint + glm::vec3(rotationA * glm::vec4(+this->mSize.x/2.0, 0.0, -this->mSize.z/2.0, 0.0)));
	glm::vec2 vertexA3 = Math::swizzleXZ(this->mCenterPoint + glm::vec3(rotationA * glm::vec4(+this->mSize.x/2.0, 0.0, +this->mSize.z/2.0, 0.0)));
	glm::vec2 vertexA4 = Math::swizzleXZ(this->mCenterPoint + glm::vec3(rotationA * glm::vec4(-this->mSize.x/2.0, 0.0, +this->mSize.z/2.0, 0.0)));
	glm::vec2 vertexB1 = Math::swizzleXZ(aOther.mCenterPoint + glm::vec3(rotationB * glm::vec4(-aOther.mSize.x/2.0, 0.0, -aOther.mSize.z/2.0, 0.0)));
	glm::vec2 vertexB2 = Math::swizzleXZ(aOther.mCenterPoint + glm::vec3(rotationB * glm::vec4(+aOther.mSize.x/2.0, 0.0, -aOther.mSize.z/2.0, 0.0)));
	glm::vec2 vertexB3 = Math::swizzleXZ(aOther.mCenterPoint + glm::vec3(rotationB * glm::vec4(+aOther.mSize.x/2.0, 0.0, +aOther.mSize.z/2.0, 0.0)));
	glm::vec2 vertexB4 = Math::swizzleXZ(aOther.mCenterPoint + glm::vec3(rotationB * glm::vec4(-aOther.mSize.x/2.0, 0.0, +aOther.mSize.z/2.0, 0.0)));

	//DEBUG
	//std::cout << vertexA1 << ' ' << vertexA2 << ' ' << vertexA3 << ' ' << vertexA4 << '\n';
	//std::cout << vertexB1 << ' ' << vertexB2 << ' ' << vertexB3 << ' ' << vertexB4 << '\n';

	std::vector<glm::vec2> axes;
	axes.reserve(8);
	//is normalized!
	axes.push_back(Math::getPerpendicularVectorFromPoints(vertexA1, vertexA2));
	axes.push_back(Math::getPerpendicularVectorFromPoints(vertexA2, vertexA3));
	axes.push_back(Math::getPerpendicularVectorFromPoints(vertexA3, vertexA4));
	axes.push_back(Math::getPerpendicularVectorFromPoints(vertexA4, vertexA1));
	axes.push_back(Math::getPerpendicularVectorFromPoints(vertexB1, vertexB2));
	axes.push_back(Math::getPerpendicularVectorFromPoints(vertexB2, vertexB3));
	axes.push_back(Math::getPerpendicularVectorFromPoints(vertexB3, vertexB4));
	axes.push_back(Math::getPerpendicularVectorFromPoints(vertexB4, vertexB1));

	for(glm::vec2& axis : axes) {
		Projection p1 = projectShape(axis, vertexA1, vertexA2, vertexA3, vertexA4);
		Projection p2 = projectShape(axis, vertexB1, vertexB2, vertexB3, vertexB4);

		if(!projectionOverlapping(p1, p2)) {
			return false; //all must be overlapping for collision
			//should work with containment
		}
	}

	//check height
	return projectionOverlapping(
		Projection(this->mCenterPoint.y-this->mSize.y/2.0, this->mCenterPoint.y+this->mSize.y/2.0),
		Projection(aOther.mCenterPoint.y-aOther.mSize.y/2.0, aOther.mCenterPoint.y+aOther.mSize.y/2.0)
	);

	//we dont care about MTV minimum translation vector
}

void BoxTrigger::setColor(const glm::vec4& aColor) noexcept {
	this->mColor = aColor;
}
void BoxTrigger::setCenterPoint(const glm::vec3& aNewPoint) noexcept {
	this->mCenterPoint = aNewPoint;
	this->mContainsNewData = true;
}
void BoxTrigger::setNewSize(const glm::vec3& aSize) noexcept {
	this->mSize = aSize;
	this->mContainsNewData = true;
}
void BoxTrigger::setRotation(const float aRotation) noexcept {
	this->mRotation = aRotation;
	this->mContainsNewData = true;
}

void BoxTrigger::enable() noexcept {
	this->mEnabled = true;
}
void BoxTrigger::disable() noexcept {
	this->mEnabled = false;
}
void BoxTrigger::toggle() noexcept {
	this->mEnabled = !this->mEnabled;
}

void BoxTrigger::show() noexcept {
	this->mVisible = true;
	this->mContainsNewData = true;
}
void BoxTrigger::hide() noexcept {
	this->mVisible = false;
	this->mContainsNewData = true;
}

BoxTrigger::~BoxTrigger() noexcept {}

InputRaycast::InputRaycast(const glm::vec3& aOrigin, const glm::vec3 aDirection) noexcept
	: mOrigin(aOrigin), mDirection(aDirection), mMinT(0.0f), mMaxT(100000.0f) {

}

//ray cast
//source http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
float InputRaycast::collision(const BoxTrigger& aBox) noexcept {
	return 0.0; //TODO
}
float InputRaycast::collision(BoxTrigger& aBox, const glm::vec4& aBoxColor) noexcept {
	float result = this->collision(aBox);
	if(result != 0.0) {
		aBox.setColor(aBoxColor);
	}
	return result;
}

void InputRaycast::setOrigin(const glm::vec3& aOrigin) noexcept {
	this->mOrigin = aOrigin;
}
void InputRaycast::setDirection(const glm::vec3& aDirection) noexcept {
	this->mDirection = aDirection;
}

InputRaycast::~InputRaycast() noexcept {

}
