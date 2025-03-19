#ifndef EUROTRAM_PHYSICS_WEATHER
#define EUROTRAM_PHYSICS_WEATHER
#include "../model/Model.hpp"
#include "../objects/Objects.hpp"
#include "../area/Map.hpp"

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

//only for rain and snow (and other falling objects)
class WeatherHandler {
public:
	WeatherHandler(const glm::vec3& aCenter, const uint64_t aDropletAmount, const float aDropletX, const float aDropletY, const glm::vec4 aDropletColor, const float aDropSpeed) noexcept;

	WeatherHandler(WeatherHandler&) noexcept = delete;
	WeatherHandler(WeatherHandler&& aOther) noexcept;
	WeatherHandler& operator=(WeatherHandler&) noexcept = delete;
	WeatherHandler& operator=(WeatherHandler&& aOther) noexcept;

	//to setup FBO
	void beginPass(Window& aWindow, UniformMat4& aProjectionUniform) noexcept;
	void endPass(Window& aWindow) noexcept;

	//advance weather movement - after FBO drawn
	void advance() noexcept;
	//for buffer @aMovementForcerLocation - write 1 if drop should move
	void draw(
		UniformMat4& aViewMatrix, UniformVec4& aColorLocation,
		const uint64_t aInstanceTranslationLocation, const uint64_t aCenterPointsLocation,
		Camera& aCamera, UniformVec3& aCameraUp, UniformVec3& aCameraRight, UniformVec3& aWeatherCenterPoint
	) noexcept;
	//move center location
	void move(const glm::vec3& aCenter) noexcept;

	~WeatherHandler() noexcept;
private:
	uint64_t mDropletAmount;
	glm::vec4 mColor;
	glm::vec3 mCenter;

	VertexArray mVAO;
	std::vector<float> mSingleDropVertices;
	VertexBuffer mSingleDropVertex;
	IndexBuffer mSingleDropIndex;

	//pass to shader
	std::vector<glm::vec4> mPositions;
	std::vector<glm::mat4> mMatrices;
	ShaderBuffer mMatrixSSBO;

	ShaderBuffer mCenterPointSSBO;
	glm::vec2 mSize;

	Framebuffer mTopView;
	Texture mDepthTopView;

	glm::mat4 mProjection, mView, mHandlerMatrix;

	float mSpeed;

	float mFrictionCoeffChange;
	//TODO other
};

//call after map initialization
void initSeasonMaterials(const Map& aMap) noexcept;
void setSeasonMaterials(const WeatherCondition aCondition) noexcept;

#endif
