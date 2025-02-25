#ifndef EUROTRAM_PHYSICS_WEATHER
#define EUROTRAM_PHYSICS_WEATHER
#include "../model/Model.hpp"
#include "../objects/Objects.hpp"

enum struct WeatherCondition : uint16_t {
	WEATHER_CLEAR          = 0,
	WEATHER_WIND           = 0b0000000000000001,
	WEATHER_RAIN           = 0b0000000000000010,
	WEATHER_LIGHTING       = 0b0000000000000100,
	WEATHER_FOG            = 0b0000000000001000,
	WEATHER_SNOW           = 0b0000000000010000,
	WEATHER_SEASONS_SPRING = 0b0001000000000000,
	WEATHER_SEASONS_SUMMER = 0b0010000000000000,
	WEATHER_SEASONS_AUTUMN = 0b0100000000000000,
	WEATHER_SEASONS_WINTER = 0b1000000000000000,
};

//https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/#solution-2--the-3d-way

//only for rain and snow (and other falling objects)
class WeatherHandler {
public:
	WeatherHandler() noexcept;

	void advance() noexcept;

	void draw() noexcept;

	~WeatherHandler() noexcept;
private:
	VertexArray mVAO;
	VertexBuffer mSingleDropVertex;
	IndexBuffer mSingleDropIndex;

	float mFrictionCoeff;
	//TODO other

	Framebuffer mTopView;
	Texture mDepthTopView;
};

void setSeasonMaterials(const WeatherCondition aCondition) noexcept;

#endif
