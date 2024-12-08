#ifndef EUROTRAM_CAMERA
#define EUROTRAM_CAMERA
#include "Window.hpp"

class Window;

class Camera {
public:
	Camera(Window* aWindow, const glm::vec3& aCameraPos, const float aFOV, const float aFarPlane, const float aSpeed) noexcept;

	void forward() noexcept;
	void back() noexcept;
	void left() noexcept;
	void right() noexcept;
	void up() noexcept;
	void down() noexcept;

	//no params - uses set speed
	//w/ params - uses argument

	void forward(const float aAmount) noexcept;
	void back(const float aAmount) noexcept;
	void left(const float aAmount) noexcept;
	void right(const float aAmount) noexcept;
	void up(const float aAmount) noexcept;
	void down(const float aAmount) noexcept;

	void moveTo(const glm::vec3& aCameraPos) noexcept;

	void setYaw(const float aYaw) noexcept;
	void setPitch(const float aPitch) noexcept;

	void addYaw(const float aYaw) noexcept;
	void addPitch(const float aPitch) noexcept;

	void setFarPlane(const float aFarPlane) noexcept;
	void setFOV(const float aFOV) noexcept;

	void setSpeed(const float aSpeed) noexcept;

	void disable() noexcept;
	void enable() noexcept;
	void toggle() noexcept;

	glm::mat4 getMatrix() const noexcept;
	glm::vec3 getPosition() const noexcept;
	glm::vec3 getDirection() const noexcept;
	float* getFOVPointer() noexcept;

	//rebuild matrices
	void update() const noexcept;

	bool isUpdated() const noexcept;

	~Camera() noexcept;
private:
	Window* mWindow;

	mutable glm::mat4 mProjection;
	mutable glm::mat4 mView;

	glm::vec3 mCameraPos;
	float mFOV;
	float mYaw;
	float mPitch;
	float mFarPlane;
	float mSpeed;

	mutable glm::vec3 mDirection;

	bool mEnabled;

	mutable bool mPrecalculated;

	glm::vec3 calculateDeltas(const float aAmount, const bool aCalculateY) noexcept;
	void updateInternal() const noexcept;
	void pitchLimiter() noexcept;
};

#endif
