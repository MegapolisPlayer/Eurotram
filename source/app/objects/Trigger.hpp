//trigger areas - if vehicle in zone, do something

#ifndef EUROTRAM_OBJECTS_TRIGGER
#define EUROTRAM_OBJECTS_TRIGGER
#include "../model/Model.hpp"

//sends collision event to respective handler if collision func successful
class AABBTriggerBox {
public:
	AABBTriggerBox() noexcept;

	void collision(AABBTriggerBox& aOther);

	~AABBTriggerBox() noexcept;
private:
	glm::vec3 point1, point2, point3, point4;
	float height;
};

#endif
