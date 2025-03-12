//trigger areas - if vehicle in zone, do something

#ifndef EUROTRAM_OBJECTS_TRIGGER
#define EUROTRAM_OBJECTS_TRIGGER
#include "../model/Model.hpp"

//TODO input ray tracing
//AABB algorithm, issue #27, #46
// www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/

struct BoxTriggerVertex {
	//position + texcoord
	float x, y, z, tc1, tc2;
};

class BoxTrigger;

class BoxTriggerDrawer {
public:
	BoxTriggerDrawer() noexcept;

	//returns id of object
	uint64_t add(BoxTrigger& aBT) noexcept;
	void remove(const uint64_t aId) noexcept;
	void draw(const uint64_t aTransformLocation, const uint64_t aColorLocation) noexcept;

	~BoxTriggerDrawer() noexcept;
private:
	VertexArray mVAO;
	VertexBuffer mVBO;
	IndexBuffer mIBO;
	std::vector<BoxTrigger*> mTriggerList;
	std::vector<glm::mat4> mTransformsData;
	ShaderBuffer mTransforms;
	ShaderBuffer mColors;

	bool mRecalculateData;
};

//to draw trigger box - render using drawer
//sends collision event to respective handler if collision func successful
class BoxTrigger {
	friend class BoxTriggerDrawer;
	friend class InputRaycast;
public:
	//cube of base vec.x x vec.z and height vec.y at point aCenter with rotation aRotation
	BoxTrigger(const glm::vec3& aCenter, const glm::vec3& aSize, const float aRotation = 0.0) noexcept;

	bool collision(BoxTrigger& aOther);

	void setColor(const glm::vec4& aColor) noexcept;
	void setCenterPoint(const glm::vec3& aNewPoint) noexcept;
	void setNewSize(const glm::vec3& aSize) noexcept;
	void setRotation(const float aRotation) noexcept;

	//disabling doesnt hide trigger!
	void enable() noexcept;
	void disable() noexcept;
	void toggle() noexcept;

	void show() noexcept;
	void hide() noexcept;

	~BoxTrigger() noexcept;
private:
	glm::vec3 mCenterPoint; //center point
	glm::vec3 mSize;
	float mRotation;
	bool mEnabled;
	bool mVisible;

	glm::vec4 mColor;

	bool mContainsNewData; //this class can only set it to true, drawer then sets to false
};

class InputRaycast {
public:
	InputRaycast(const glm::vec3& aOrigin, const glm::vec3 aDirection) noexcept;

	//returns where exactly along ray collided or 0 if didnt
	float collision(const BoxTrigger& aBox) noexcept;
	//same as previous, but also sets the color of the box on collision
	float collision(BoxTrigger& aBox, const glm::vec4& aBoxColor) noexcept;

	void setOrigin(const glm::vec3& aOrigin) noexcept;
	void setDirection(const glm::vec3& aDirection) noexcept;

	~InputRaycast() noexcept;
private:
	glm::vec3 mOrigin;
	glm::vec3 mDirection;

	//maximum points on ray when to count intersection
	float mMinT;
	float mMaxT;
};

#endif
