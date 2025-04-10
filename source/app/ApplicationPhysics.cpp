#include "Application.hpp"

//returns false on error
bool Application::runPhysics() noexcept {
	this->mWeatherEffect.move(glm::vec3(this->mCamera.getPosition().x, 0.0, this->mCamera.getPosition().z));
	this->mWeatherEffect.advance();
	setSeasonMaterials((WeatherCondition)this->mLine.getWeather());

	for(Vehicle& v : this->mPlayerVehicles) {
		if(!v.physicsUpdate(this->mLine.getWeather(), 1.0/this->mPhysicalUpdateFreq)) return false;
		if(!v.update(this->mMap, this->mLine)) return false;
	}
	return true;
}
