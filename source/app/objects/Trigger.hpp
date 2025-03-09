//trigger areas - if vehicle in zone, do something

#ifndef EUROTRAM_OBJECTS_TRIGGER
#define EUROTRAM_OBJECTS_TRIGGER
#include "../model/Model.hpp"

//TODO input ray tracing
//AABB algorithm, issue #27, #46
// www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/

//sends collision event to respective handler if collision func successful
class AABBTriggerBox {
public:
	AABBTriggerBox(glm::vec3& aCenter, const float aWidth, const float aDepth, const float aHeight) noexcept;

	bool collision(AABBTriggerBox& aOther);
	void draw() noexcept;

	void setColor(glm::vec4& aColor) noexcept;



	~AABBTriggerBox() noexcept;
private:
	glm::vec3 mCenterPoint; //center point
	float mWidth, mDepth, mHeight; //size
	bool mEnabled;
	bool mVisible;

	glm::vec4 mColor;
};

class InputRaycast {
public:
	InputRaycast() noexcept;

	float collision(AABBTriggerBox& aBox);

	~InputRaycast() noexcept;
private:
	glm::vec3 mOrigin;
	glm::vec3 mDirection;

	//maximum points on ray when to count intersection
	float mMinT;
	float mMaxT;
};

#endif
