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

//box of size 1x1 - we scale
static std::vector<float> WEATHER_VERTICES_BASE = {
	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
};
static GLuint WEATHER_INDICES[] = {
	0, 1, 2, 2, 3, 0
};

constexpr uint64_t WEATHER_FBO_TEXTURE_SIZE = 1024;
constexpr float WEATHER_HEIGHT = 25.0f;

WeatherHandler::WeatherHandler(const glm::vec3& aCenter, const uint64_t aDropletAmount, const float aDropletX, const float aDropletY, const glm::vec4 aDropletColor) noexcept
	: mDropletAmount(aDropletAmount), mColor(aDropletColor), mCenter(aCenter), mSingleDropVertex(nullptr, 4, 5), mSingleDropIndex(WEATHER_INDICES, 6),
	mPositions(this->mDropletAmount), mMatrices(this->mDropletAmount), mMatrixSSBO(nullptr, 0),
	mCenterPointSSBO(nullptr, 0), mSize(0.5f*2*aDropletX, 0.5f*2*aDropletY),
	mDepthTopView(WEATHER_FBO_TEXTURE_SIZE, WEATHER_FBO_TEXTURE_SIZE, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, true, TextureScale::NEAREST_NEIGHBOR, TextureBorder::FILL_OUT_OF_RANGE),
	mProjection(1.0), mView(1.0), mHandlerMatrix(1.0), mFrictionCoeffChange(0.0) {
	this->mPositions.resize(this->mDropletAmount); //should not result in realloc - we set in member init list
	this->mMatrices.resize(this->mDropletAmount);
	this->mMatrixSSBO = ShaderBuffer(this->mMatrices.data(), this->mMatrices.size()*sizeof(glm::mat4));
	for(uint64_t i = 0; i < this->mDropletAmount; i++) {
		this->mPositions[i] = glm::vec4(
			Math::getRandomNumber(-WEATHER_HEIGHT/2.0, WEATHER_HEIGHT/2.0),
			Math::getRandomNumber(-WEATHER_HEIGHT, WEATHER_HEIGHT),
			Math::getRandomNumber(-WEATHER_HEIGHT/2.0, WEATHER_HEIGHT/2.0),
			0.0
		);
		this->mMatrices[i] = glm::translate(glm::mat4(1.0), this->mCenter+glm::vec3(this->mPositions[i]));
	}

	this->mCenterPointSSBO = ShaderBuffer(this->mPositions.data(), this->mDropletAmount*sizeof(glm::vec4));

	{
		std::vector<float> localWeatherVerticesCopy = WEATHER_VERTICES_BASE;
		for(uint64_t i = 0; i < localWeatherVerticesCopy.size(); i+=5) {
			localWeatherVerticesCopy[i+0] *= aDropletX;
			localWeatherVerticesCopy[i+1] *= aDropletY;
		}
		this->mSingleDropVertices = std::move(localWeatherVerticesCopy);
	}

	this->mDepthTopView.setOutOfBoundsColor(1.0, 1.0, 1.0); //out of bounds

	this->mVAO.bind();
	this->mSingleDropVertex = VertexBuffer((float*)this->mSingleDropVertices.data(), 4, 5);
	this->mSingleDropVertex.enableAttribute(&this->mVAO, 3); //pos
	this->mSingleDropVertex.enableAttribute(&this->mVAO, 2); //texcoord

	//looking straight down - change up vector
	this->mView = glm::lookAt(glm::vec3(aCenter.x, WEATHER_HEIGHT+aCenter.y, aCenter.z), aCenter, glm::vec3(1.0f, 0.0f, 0.0f));
	this->mProjection = glm::ortho(-WEATHER_HEIGHT/2.0f, WEATHER_HEIGHT/2.0f, -WEATHER_HEIGHT/2.0f, WEATHER_HEIGHT/2.0f, 0.1f, 2.0f*WEATHER_HEIGHT);
	this->mHandlerMatrix = this->mProjection * this->mView;

	this->mTopView.bind();
	this->mTopView.bindTexture(this->mDepthTopView, GL_DEPTH_ATTACHMENT);

	this->mTopView.checkStatus();
	this->mTopView.unbind();
}
WeatherHandler::WeatherHandler(WeatherHandler&& aOther) noexcept
	: mDropletAmount(std::move(aOther.mDropletAmount)), mColor(std::move(aOther.mColor)), mCenter(std::move(aOther.mCenter)), mVAO(std::move(aOther.mVAO)),
	mSingleDropVertex(std::move(aOther.mSingleDropVertex)), mSingleDropIndex(std::move(aOther.mSingleDropIndex)),
	mPositions(std::move(aOther.mPositions)), mMatrices(std::move(aOther.mMatrices)), mMatrixSSBO(std::move(aOther.mMatrixSSBO)),
	mCenterPointSSBO(std::move(aOther.mCenterPointSSBO)), mSize(std::move(aOther.mSize)),
	mTopView(std::move(aOther.mTopView)), mDepthTopView(std::move(aOther.mDepthTopView)),
	mProjection(std::move(aOther.mProjection)), mView(std::move(aOther.mView)), mHandlerMatrix(std::move(aOther.mHandlerMatrix)),
	mFrictionCoeffChange(std::move(aOther.mFrictionCoeffChange)) {}
WeatherHandler& WeatherHandler::operator=(WeatherHandler&& aOther) noexcept {
	this->mDropletAmount = std::move(aOther.mDropletAmount);
	this->mColor = std::move(aOther.mColor);
	this->mCenter = std::move(aOther.mCenter);
	this->mVAO = std::move(aOther.mVAO);
	this->mSingleDropVertex = std::move(aOther.mSingleDropVertex);
	this->mSingleDropIndex = std::move(aOther.mSingleDropIndex);
	this->mPositions = std::move(aOther.mPositions);
	this->mMatrices = std::move(aOther.mMatrices);
	this->mMatrixSSBO = std::move(aOther.mMatrixSSBO);
	this->mCenterPointSSBO = std::move(aOther.mCenterPointSSBO);
	this->mSize = std::move(aOther.mSize);
	this->mTopView = std::move(aOther.mTopView);
	this->mDepthTopView = std::move(aOther.mDepthTopView);
	this->mProjection = std::move(aOther.mProjection);
	this->mView = std::move(aOther.mView);
	this->mHandlerMatrix = std::move(aOther.mHandlerMatrix);
	this->mFrictionCoeffChange = std::move(aOther.mFrictionCoeffChange);

	return *this;
}

void WeatherHandler::beginPass(Window& aWindow, UniformMat4& aProjectionUniform) noexcept {
	aWindow.setViewport(WEATHER_FBO_TEXTURE_SIZE, WEATHER_FBO_TEXTURE_SIZE);
	this->mTopView.bind();
	this->mTopView.bindTexture(this->mDepthTopView, GL_DEPTH_ATTACHMENT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	this->mHandlerMatrix = this->mProjection * this->mView;
	aProjectionUniform.set(this->mHandlerMatrix);
}
void WeatherHandler::endPass(Window& aWindow) noexcept {
	this->mTopView.unbind();
	aWindow.resetViewport();
}

void WeatherHandler::advance(const float aSpeed) noexcept {
	for(uint64_t i = 0; i < this->mDropletAmount; i++) {
		//if depth smaller than value in DB (not deutsche bahn but depth buffer)
		//we attemped to set this value in shader - WE CANNOT BECAUSE SOME FRAGMENTS GET DISCARDED AND THEY DONT RUN
		//everything on CPU (too lazy to use compute)
		if(this->mPositions[i].y <= this->mCenter.y-WEATHER_HEIGHT) {
			this->mPositions[i].y += 2*WEATHER_HEIGHT;
		}
		this->mPositions[i].y -= aSpeed;
		this->mMatrices[i] = glm::translate(glm::mat4(1.0), this->mCenter+glm::vec3(this->mPositions[i]));
	}
}
void WeatherHandler::draw(
	UniformMat4& aViewMatrix, UniformVec4& aColorLocation,
	const uint64_t aInstanceTranslationLocation, const uint64_t aCenterPointsLocation,
	Camera& aCamera, UniformVec3& aCameraUp, UniformVec3& aCameraRight, UniformVec3& aWeatherCenterPoint
	) noexcept {
	this->mDepthTopView.bind(0);
	this->mHandlerMatrix = this->mProjection * this->mView;
	aViewMatrix.set(this->mHandlerMatrix);
	aColorLocation.set(this->mColor);
	aCameraUp.set(aCamera.getWorldSpaceUp());
	aCameraRight.set(aCamera.getWorldSpaceRight());
	aWeatherCenterPoint.set(this->mCenter);
	this->mMatrixSSBO.update(this->mMatrices.data(), this->mMatrices.size()*sizeof(glm::mat4));
	this->mMatrixSSBO.bind(aInstanceTranslationLocation);
	this->mCenterPointSSBO.update(this->mPositions.data(), this->mPositions.size()*sizeof(glm::vec4));
	this->mCenterPointSSBO.bind(aCenterPointsLocation);

	this->mVAO.bind();
	this->mSingleDropVertex.bind();
	this->mSingleDropIndex.bind();
	this->mSingleDropIndex.drawInstanced(this->mDropletAmount);
}
//TODO check when move ends (aCenter doesnt change)
//TODO when moving dont move drops
void WeatherHandler::move(const glm::vec3& aCenter) noexcept {
	this->mCenter = aCenter;
	this->mView = glm::lookAt(glm::vec3(aCenter.x, WEATHER_HEIGHT+aCenter.y, aCenter.z), aCenter, glm::vec3(1.0f, 0.0f, 0.0f));
	//projection stays
	this->mHandlerMatrix = this->mProjection * this->mView;
}

WeatherHandler::~WeatherHandler() noexcept {}
