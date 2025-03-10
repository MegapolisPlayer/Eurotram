#include "Trigger.hpp"

//as is usual: simple 1x1x1 cube in VBO, then we scale and translate

//TODO base vertices
//TODO AABBTriggerBoxDrawer

AABBTriggerBox::AABBTriggerBox(const glm::vec3& aCenter, const glm::vec3& aSize, const float aRotation) noexcept {}

bool AABBTriggerBox::collision(AABBTriggerBox& aOther) {
	return false; //TODO
}
void AABBTriggerBox::setColor(glm::vec4& aColor) noexcept {

}

void AABBTriggerBox::setCenterPoint(const glm::vec3& aNewPoint) noexcept {

}
void AABBTriggerBox::setNewSize(const glm::vec3& aSize) noexcept {

}
void AABBTriggerBox::setRotation(const float aRotation) noexcept {

}

void AABBTriggerBox::enable() noexcept {

}
void AABBTriggerBox::disable() noexcept {

}
void AABBTriggerBox::toggle() noexcept {

}

void AABBTriggerBox::show() noexcept {

}
void AABBTriggerBox::hide() noexcept {

}

AABBTriggerBox::~AABBTriggerBox() noexcept {

}

InputRaycast::InputRaycast(const glm::vec3& aOrigin, const glm::vec3 aDirection) noexcept {

}

float InputRaycast::collision(const AABBTriggerBox& aBox) noexcept {
	return 0.0; //TODO
}
float InputRaycast::collision(AABBTriggerBox& aBox, const glm::vec4& aBoxColor) noexcept {
	return 0.0; //TODO
}

void InputRaycast::setOrigin(const glm::vec3& aOrigin) noexcept {

}
void InputRaycast::setDirection(const glm::vec3& aDirection) noexcept {

}

InputRaycast::~InputRaycast() noexcept {

}
