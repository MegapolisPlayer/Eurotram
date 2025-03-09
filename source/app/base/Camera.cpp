#include "Camera.hpp"

//definition of window update func
void Window::updateCamera() noexcept {
	if(this->mReferencedCamera != nullptr) {
		this->mReferencedCamera->update();
	}
}

//so we start oriented correctly: 0 is to the right (+X axis), 90 is back
Camera::Camera(Window* aWindow, const glm::vec3& aCameraPos, const float aFOV, const float aFarPlane, const float aSpeed) noexcept
	: mWindow(aWindow), mCameraPos(aCameraPos), mFOV(aFOV), mFarPlane(aFarPlane), mSpeed(aSpeed), mPitch(0.0), mYaw(-90.0), mDirection(1.0f), mEnabled(true), mPrecalculated(false) {
	this->update();
	this->mWindow->bindCamera(this);
}

//point [x,y] rotated 90deg
//CW, to right: [-y, x]
//CCW, to left: [y, -x]
//y is z here

//left = -y = x => y = -x
//        x = y

//right = y = x
//       -x = y => x = -y

void Camera::forward() noexcept {
	this->mPrecalculated = false;
	this->mCameraPos += this->calculateDeltas(this->mSpeed, true);
}
void Camera::back() noexcept {
	this->mPrecalculated = false;
	this->mCameraPos -= this->calculateDeltas(this->mSpeed, true);
}
void Camera::left() noexcept {
	this->mPrecalculated = false;
	glm::vec3 vec = this->calculateDeltas(this->mSpeed, false);
	this->mCameraPos.x += vec.z;
	this->mCameraPos.z -= vec.x;
}
void Camera::right() noexcept {
	this->mPrecalculated = false;
	glm::vec3 vec = this->calculateDeltas(this->mSpeed, false);
	this->mCameraPos.x -= vec.z;
	this->mCameraPos.z += vec.x;
}
void Camera::up() noexcept {
	this->mPrecalculated = false;
	this->mCameraPos.y += this->mSpeed;
}
void Camera::down() noexcept {
	this->mPrecalculated = false;
	this->mCameraPos.y -= this->mSpeed;
}

void Camera::forward(const float aAmount) noexcept {
	this->mPrecalculated = false;
	this->mCameraPos += this->calculateDeltas(aAmount, true);
}
void Camera::back(const float aAmount) noexcept {
	this->mPrecalculated = false;
	this->mCameraPos -= this->calculateDeltas(aAmount, true);
}
void Camera::left(const float aAmount) noexcept {
	this->mPrecalculated = false;
	glm::vec3 vec = this->calculateDeltas(aAmount, false);
	this->mCameraPos.x += vec.z;
	this->mCameraPos.z -= vec.x;
}
void Camera::right(const float aAmount) noexcept {
	this->mPrecalculated = false;
	glm::vec3 vec = this->calculateDeltas(aAmount, false);
	this->mCameraPos.x -= vec.z;
	this->mCameraPos.z += vec.x;
}
void Camera::up(const float aAmount) noexcept {
	this->mPrecalculated = false;
	this->mCameraPos.y += aAmount;
}
void Camera::down(const float aAmount) noexcept {
	this->mPrecalculated = false;
	this->mCameraPos.y -= aAmount;
}

void Camera::moveTo(const glm::vec3& aCameraPos) noexcept {
	this->mPrecalculated = false;
	this->mCameraPos = aCameraPos;
}

void Camera::setYaw(const float aYaw) noexcept {
	this->mPrecalculated = false;
	this->mYaw = aYaw;
}
void Camera::setPitch(const float aPitch) noexcept {
	this->mPrecalculated = false;
	this->mPitch = aPitch;
	this->pitchLimiter();
}
void Camera::addYaw(const float aYaw) noexcept {
	this->mPrecalculated = false;
	this->mYaw += aYaw;
}
void Camera::addPitch(const float aPitch) noexcept {
	this->mPrecalculated = false;
	this->mPitch += aPitch;
	this->pitchLimiter();
}

void Camera::setFarPlane(const float aFarPlane) noexcept {
	this->mPrecalculated = false;
	this->mFarPlane = aFarPlane;
}
void Camera::setFOV(const float aFOV) noexcept {
	this->mFOV = aFOV;
}

void Camera::disable() noexcept {
	this->mEnabled = false;
}
void Camera::enable() noexcept {
	this->mEnabled = true;
}
void Camera::toggle() noexcept {
	this->mEnabled = !this->mEnabled;
}

glm::mat4 Camera::getMatrix() const noexcept {
	this->update();
	return this->mProjection * this->mView;
}
glm::vec3 Camera::getPosition() const noexcept {
	return this->mCameraPos;
}
glm::vec3 Camera::getDirection() const noexcept {
	this->update();
	return this->mDirection;
}
float* Camera::getFOVPointer() noexcept {
	return &this->mFOV;
}

void Camera::update() const noexcept {
	//we have no info about FOV changes
	this->mProjection = glm::perspective(glm::radians((float)this->mFOV), (float)this->mWindow->mWidth/this->mWindow->mHeight, 0.1f, this->mFarPlane);

	if(this->mPrecalculated) return;

	this->mDirection = glm::vec3();
	//first part - trigoniometry (where are we looking)
	//multiplied because affected by Y movement
	this->mDirection.x = cos(glm::radians(this->mYaw)) * cos(glm::radians(this->mPitch));
	this->mDirection.y = sin(glm::radians(this->mPitch));
	this->mDirection.z = sin(glm::radians(this->mYaw)) * cos(glm::radians(this->mPitch));
	this->mDirection = glm::normalize(this->mDirection);

	this->mView = glm::lookAt(this->mCameraPos, this->mCameraPos + this->mDirection, glm::vec3(0.0, 1.0, 0.0));

	this->mPrecalculated = true;
}

bool Camera::isUpdated() const noexcept {
	return this->mPrecalculated;
}

glm::vec3 Camera::getWorldSpaceUp() const noexcept {
	this->update();
	return glm::vec3(this->mView[0][1], this->mView[1][1], this->mView[2][1]);
}
glm::vec3 Camera::getWorldSpaceRight() const noexcept {
	this->update();
	return glm::vec3(this->mView[0][0], this->mView[1][0], this->mView[2][0]);
}

Camera::~Camera() noexcept {
	this->mWindow->bindCamera(NULL);
}

glm::vec3 Camera::calculateDeltas(const float aAmount, const bool aCalculateY) noexcept {
	glm::vec3 result;
	result.x = std::cos(glm::radians(this->mYaw))*aAmount;
	result.z = std::sin(glm::radians(this->mYaw))*aAmount;
	if(aCalculateY) {
		result.y = std::sin(glm::radians(this->mPitch))*aAmount;
	}
	else {
		result.y = 0.0f;
	}
	return result;
}

void Camera::updateInternal() const noexcept {
	if(!this->mEnabled) return;
	this->update();
}

#define CAMERA_PITCH_LIMIT_PLUS 90.0f
#define CAMERA_PITCH_LIMIT_MINUS -90.0f

void Camera::pitchLimiter() noexcept {
	if(this->mPitch > CAMERA_PITCH_LIMIT_PLUS) this->mPitch = CAMERA_PITCH_LIMIT_PLUS;
	if(this->mPitch < CAMERA_PITCH_LIMIT_MINUS) this->mPitch = CAMERA_PITCH_LIMIT_MINUS;
}
