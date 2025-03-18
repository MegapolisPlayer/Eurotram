#include "Application.hpp"

//returns false on error
bool Application::runPhysics() noexcept {
	for(Vehicle& v : this->mPlayerVehicles) {
		v.physicsUpdate(1.0/this->mPhysicalUpdateFreq);
	}
	return true;
}
