#include "Weather.hpp"

//TODO
void setSeasonMaterials(const WeatherCondition aCondition) noexcept {
		if(((uint16_t)aCondition & (uint16_t)WeatherCondition::WEATHER_SEASONS_SPRING) > 0) {

		}
		if(((uint16_t)aCondition & (uint16_t)WeatherCondition::WEATHER_SEASONS_SUMMER) > 0) {

		}
		if(((uint16_t)aCondition & (uint16_t)WeatherCondition::WEATHER_SEASONS_AUTUMN) > 0) {

		}
		if(((uint16_t)aCondition & (uint16_t)WeatherCondition::WEATHER_SEASONS_WINTER) > 0) {

		}
}
