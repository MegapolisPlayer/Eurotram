#ifndef EUROTRAM_PHYSICS_UPDATE
#define EUROTRAM_PHYSICS_UPDATE
#include "Curve.hpp"
#include "Drag.hpp"
#include "ElectricalCircuit.hpp"
#include "Force.hpp"
#include "Friction.hpp"
#include "Nadal.hpp"
#include "Weather.hpp"

//runs every 20 ms / 50x a second

typedef void(*PhysicsUpdateFunction)();

class PhysicsUpdater {
public:
private:
	WeatherCondition mWeather;
};

#endif
