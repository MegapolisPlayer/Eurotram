//trigger areas - if vehicle in zone, do something

#ifndef EUROTRAM_OBJECTS_TRIGGER
#define EUROTRAM_OBJECTS_TRIGGER
#include "../model/Model.hpp"

//sends collision event to respective handler if collision func successful
class AABBTriggerBox {
public:
	AABBTriggerBox() noexcept;

	bool collision(AABBTriggerBox& aOther);

	void makeVisible() noexcept;

	~AABBTriggerBox() noexcept;
private:
	glm::vec3 mCenterPoint; //center point
	float mWidth, mDepth, mHeight; //size
	bool mEnabled;
	bool mVisible;
};

class AABBTriggerBoxDrawer {
public:
private:

};

#endif
