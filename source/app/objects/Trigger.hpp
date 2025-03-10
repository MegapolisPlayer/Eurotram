//trigger areas - if vehicle in zone, do something

#ifndef EUROTRAM_OBJECTS_TRIGGER
#define EUROTRAM_OBJECTS_TRIGGER
#include "../model/Model.hpp"

//TODO input ray tracing
//AABB algorithm, issue #27, #46
// www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/

struct AABBTriggerBoxVertex {
	//position + texcoord
	float x, y, z, tc1, tc2;
};

class AABBTriggerBoxDrawer {
public:
	AABBTriggerBoxDrawer() noexcept;

	//returns id of object
	uint64_t add() noexcept;
	void remove(const uint64_t aId) noexcept;
	void draw() noexcept;

	void show(const uint64_t aId) noexcept; //TODO set scale to 0
	void hide(const uint64_t aId) noexcept;

	~AABBTriggerBoxDrawer() noexcept;
private:
	uint64_t mAmountBoxes;

	VertexArray mVAO;
	VertexBuffer mVBO;
	IndexBuffer mIBO;
	ShaderBuffer mTransforms;
};

//to draw trigger box - render using drawer
//sends collision event to respective handler if collision func successful
class AABBTriggerBox {
	friend class AABBTriggerBoxDrawer;
	friend class InputRaycast;
public:
	//cube of base vec.x x vec.z and height vec.y at point aCenter with rotation aRotation
	AABBTriggerBox(const glm::vec3& aCenter, const glm::vec3& aSize, const float aRotation = 0.0) noexcept;

	bool collision(AABBTriggerBox& aOther);
	void setColor(glm::vec4& aColor) noexcept;

	void setCenterPoint(const glm::vec3& aNewPoint) noexcept;
	void setNewSize(const glm::vec3& aSize) noexcept;
	void setRotation(const float aRotation) noexcept;

	void enable() noexcept;
	void disable() noexcept;
	void toggle() noexcept;

	void show() noexcept;
	void hide() noexcept;

	~AABBTriggerBox() noexcept;
private:
	glm::vec3 mCenterPoint; //center point
	glm::vec3 mSize;
	float mRotation;
	bool mEnabled;
	bool mVisible;

	glm::vec4 mColor;
};

class InputRaycast {
public:
	InputRaycast(const glm::vec3& aOrigin, const glm::vec3 aDirection) noexcept;

	//returns where exactly along ray collided or 0 if didnt
	float collision(const AABBTriggerBox& aBox) noexcept;
	//same as previous, but also sets the color of the box on collision
	float collision(AABBTriggerBox& aBox, const glm::vec4& aBoxColor) noexcept;

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
