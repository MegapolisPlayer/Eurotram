#include "Application.hpp"

//returns false on error
bool Application::runPhysics() noexcept {
	for(Vehicle& v : mPlayerVehicles) {
		v.update(this->mMap, this->mLine);
	}
	return true; //TODO
}
