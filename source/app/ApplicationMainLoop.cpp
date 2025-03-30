#include "Application.hpp"
#include "imgui.h"

static bool FirstMove = true;
static double LastX = 1920/2, LastY = 1080/2;

static glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 20.0f);
static bool moveWithSpotlight = true;

static bool hideGui = false;
static bool cameraFollowsVehicle = false;

static float* vehicleThrottleRef = nullptr;
static Line* lineRef = nullptr;
static Vehicle* vehicleRef = nullptr;

static bool lastButton = false;

void Application::rawKeyCallback(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) noexcept {
	if(aKey == GLFW_KEY_G) {
		if(aAction == GLFW_RELEASE) {
			lastButton = false;
		}
		else if (aAction == GLFW_PRESS) {
			if(!lastButton) {
				lastButton = true;
				vehicleRef->confirmAnnouncement();
				lineRef->playCurrentAnnouncement();
			}
		}
		return;
	}

	if(aAction == GLFW_RELEASE || !aWindow->isCursorHidden()) return;

	switch(aKey) {
		case GLFW_KEY_W:
			aWindow->getCamera()->forward();
			break;
		case GLFW_KEY_A:
			aWindow->getCamera()->left();
			break;
		case GLFW_KEY_S:
			aWindow->getCamera()->back();
			break;
		case GLFW_KEY_D:
			aWindow->getCamera()->right();
			break;
		case GLFW_KEY_R:
			aWindow->getCamera()->up();
			break;
		case GLFW_KEY_F:
			aWindow->getCamera()->down();
			break;
		case GLFW_KEY_Q:
			aWindow->getCamera()->moveTo({0.0f, 0.0f, 0.0f});
			break;
		case GLFW_KEY_C:
			cameraFollowsVehicle = !cameraFollowsVehicle;
			break;
		case GLFW_KEY_X:
			aWindow->close();
			break;
		case GLFW_KEY_M:
			moveWithSpotlight = !moveWithSpotlight;
			break;
		case GLFW_KEY_F1:
			hideGui = !hideGui;
			break;
		case GLFW_KEY_P:
			vehicleRef->getVehicleControlData()->sander = !vehicleRef->getVehicleControlData()->sander;
			break;
		case GLFW_KEY_B:
			vehicleRef->getSoundSimulation()->toggleBell();
			break;
		case GLFW_KEY_Z:
			vehicleRef->getSoundSimulation()->toggleInfo();
			break;
		case GLFW_KEY_UP:
			*vehicleThrottleRef += 0.01;
			if(*vehicleThrottleRef >= 1.0) *vehicleThrottleRef = 1.0;
			break;
		case GLFW_KEY_DOWN:
			*vehicleThrottleRef -= 0.01;
			if(*vehicleThrottleRef <= -1.0) *vehicleThrottleRef = -1.0;
			break;
	}
}
void Application::rawMouseCallback(Window* aWindow, double aX, double aY) noexcept {
	if(!aWindow->isCursorHidden()) return;

	if(FirstMove) {
		LastX = aX;
		LastY = aY;
		FirstMove = false;
	}

	double DX = aX - LastX;
	double DY = LastY - aY;

	LastX = aX;
	LastY = aY;

	double Sensitivity = 0.1f;
	DX *= Sensitivity;
	DY *= Sensitivity;

	aWindow->getCamera()->addYaw(DX);
	aWindow->getCamera()->addPitch(DY);
}
void Application::rawClickCallback(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) noexcept {
	if(aKey == GLFW_MOUSE_BUTTON_RIGHT && aAction == GLFW_PRESS) {
		aWindow->hideCursor();
	}
}

bool Application::runInternal() noexcept {
	lineRef = &this->mLine;

	Shader shader("shader/vertex.glsl", "shader/fragment.glsl");
	shader.bind();

	UniformInt uOITEnabled(37);
	UniformInt uIgnoreLighting(38);

	TextureSamplerArray<32> tsa(232);
	tsa.set();

	UniformMat4 matCameraUniform(10);
	matCameraUniform.set(this->mCamera.getMatrix());
	UniformVec3 cameraPosUniform(210);
	UniformMat4 matModelUniform(11);
	UniformMat3 matNormalUniform(12);

	Dirlight d;
	d.color = {208.0f/255.0f, 128.0f/255.0f, 0.0f, 1.0f};
	glm::mat4 rotationMatrix = glm::mat4(1.0);
	d.direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);

	DirectionalShadows ds(lightPos, glm::vec3(0.0f), 100.0f, 0.1f, 300.0f);

	UniformMat4 spu(13);
	spu.set(ds.getProjectionMatrix());

	UniformDirlight uDirlight(51);
	uDirlight.update(&d);
	uDirlight.set();

	//Pointlight p;
	//p.color = {1.0f, 0.0f, 1.0f, 1.0f};
	//p.position = {0.0f, 5.0f, 0.0f, 0.0f};
	//setAttenuation(AttenuationValues::DISTANCE_100, &p.constant, &p.linear, &p.quadratic);

	Spotlight s;
	s.color = {1.0f, 1.0f, 1.0f, 1.0f};
	s.lightType = LIGHT_TYPE_FLASHLIGHT;

	setAttenuation(AttenuationValues::DISTANCE_100, &s.constant, &s.linear, &s.quadratic);
	setSpotlightRadius(&s, 15.0, 10.0);

	//UniformPointlight uPoints(52, 0);
	//uPoints.update(&p);
	//uPoints.set();

	UniformSpotlight uSpots(53, 1);
	uSpots.update(&s);
	uSpots.set();

	SpotlightShadows ss(lightPos, s.direction, 0.1, 30.0, *this->mCamera.getFOVPointer());
	UniformMat4 fpu(14);
	fpu.set(ss.getProjectionMatrix());

	UniformFloat ambientLightStrength(200);
	ambientLightStrength.set(this->mAmbient);

	float sunAngle = 0;

	//TODO starting track can be shorter than 6.45 meter

	Vehicle& v = this->mPlayerVehicles.emplace_back("t3rp.cfg");

	std::cout << "Loading T3R.P model...\n";
	//Model t3rp(std::filesystem::path("./untitled.glb"));
	Model t3rp(std::filesystem::path(v.getConfigModelFilename()));
	std::cout << "Model loaded!\n";

	v.init(this->mMap, this->mLine, &t3rp);
	vehicleThrottleRef = &v.getVehicleControlData()->throttle;
	*vehicleThrottleRef = 0.0;
	vehicleRef = &v;

	shader.bind();
	UniformMaterial uMaterial(50);
	StructUniform<glm::mat4> uModelMat(40, 0);

	UniformInt uIsInstancedRendering(36);

	Shader shadowMapProgram("shader/shadowVertex.glsl", "shader/shadowFragment.glsl");
	shadowMapProgram.bind();
	UniformInt luIsInstancedRendering(36);
	UniformMat4 lpu(90);
	UniformMat4 lmod(91);
	StructUniform<glm::mat4> lmat(40, 0);

	std::cout << "GMS length " << GlobalMaterialStore::getLength() << '\n';
	uMaterial.setNewData(nullptr, GlobalMaterialStore::getLength());

	GMSEntry* nightWindows = GlobalMaterialStore::addVariant("Material.windowDay", "NIGHT");
	nightWindows->material.color = glm::vec4(231.0/255.0, 226.0/255.0, 19.0/255.0, 1.0);
	nightWindows->material.brightness = MAX_BRIGHTNESS;
	nightWindows->material.specular = glm::vec4(0.0);

	GMSEntry* nightStation = GlobalMaterialStore::addVariant("Material.symbol", "NIGHT");
	nightStation->material.brightness = MAX_BRIGHTNESS;
	nightStation->material.specular = glm::vec4(0.0);

	ScreenRenderer sr;
	OIT oit(this->mWindow);

	Shader weatherShader("shader/weatherVertex.glsl", "shader/weatherFragment.glsl");
	weatherShader.bind();
	UniformMat4 whCameraMatrix(34);
	UniformVec4 whColor(30);
	UniformFloat whAmbient(31);
	UniformMat4 whViewMatrix(36);
	UniformVec2 whSize(38);
	UniformVec3 whCamUp(39);
	UniformVec3 whCamRight(40);
	UniformVec3 whWeatherCenter(41);

	Shader btShader("shader/btVertex.glsl", "shader/btFragment.glsl");
	UniformMat4 btCamera(30);

	InputRaycast ir(glm::vec3(0.0), glm::vec3(0.0));

	glm::vec3 oldVehicleRot = glm::vec3(0.0);

	this->runWindowFrame([&]() {
		//std::cout << "THR " << v.getVehicleControlData()->throttle << '\n';

		//vehicle updated in physics loop
		if(cameraFollowsVehicle) {
			this->mCamera.moveTo(v.getCameraPosition());
			glm::vec3 cameraVehicleRot = v.getCameraRotation();
			this->mCamera.addYaw(cameraVehicleRot.y-oldVehicleRot.y);
			this->mCamera.addPitch(cameraVehicleRot.x-oldVehicleRot.x);
			oldVehicleRot = cameraVehicleRot;
		}
		else {
			oldVehicleRot = glm::vec3(0.0);
		}

		this->mMap.regenerateInstanceArray(toStationCode("ZELV"), toStationCode("OLSH"), toStationCode("FLOR"), toStationCode("RADH"));

		shadowMapProgram.bind();
		t3rp.sendAnimationDataToShader(lmat);

		//write to depth buffer
		this->mWeatherEffect.beginPass(this->mWindow, lpu);
		t3rp.draw(uMaterial, lmat, &lmod);
		this->mMap.draw(uMaterial, lmat, 35, luIsInstancedRendering, &lmod);
		this->mWeatherEffect.endPass(this->mWindow);

		ss.beginPass(this->mWindow, lpu);
		t3rp.draw(uMaterial, lmat, &lmod);
		this->mMap.draw(uMaterial, lmat, 35, luIsInstancedRendering, &lmod);
		ss.endPass(this->mWindow);

		ds.beginPass(this->mWindow, lpu);
		t3rp.draw(uMaterial, lmat, &lmod);
		this->mMap.draw(uMaterial, lmat, 35, luIsInstancedRendering, &lmod);
		ds.endPass(this->mWindow);

		// main draw

		shader.bind();
		spu.set(ds.getProjectionMatrix());
		fpu.set(ss.getProjectionMatrix());

		if(moveWithSpotlight) {
			s.position = glm::vec4(this->mWindow.getCamera()->getPosition(), 1.0);
			s.direction = glm::vec4(this->mWindow.getCamera()->getDirection(), 1.0);
		}
		uSpots.update(&s);
		ss.setPosDirection(s.position, s.direction);
		ir.setOrigin(s.position); //update ray
		ir.setDirection(s.direction);

		for(BoxTrigger& bt : v.getTriggers()) {
			ir.collision(bt, glm::vec4(0.0, 1.0, 0.0, 0.5), glm::vec4(1.0, 0.0, 0.0, 0.5));
		}

		matCameraUniform.set(this->mWindow.getCamera()->getMatrix());
		cameraPosUniform.set(this->mWindow.getCamera()->getPosition());

		ss.bindMap(28); //28 flashlight
		ds.bindMap(31); //31 sun
		//29,30 front lights of tram

		//t3rp.setAnimation("pantographAction", std::fmod(glfwGetTime(), 3.3));
		//t3rp.setAnimation("driverDoorAction", std::fmod(glfwGetTime(), 3.3));

		shader.bind();

		oit.beginOpaquePass(this->mWindow, uOITEnabled);
		t3rp.draw(uMaterial, uModelMat, &matModelUniform, &matNormalUniform);
		this->mMap.draw(uMaterial, uModelMat, 35, uIsInstancedRendering, &matModelUniform, &matNormalUniform);
		oit.endOpaquePass(uOITEnabled);

		oit.beginTransparentPass(uOITEnabled);
		t3rp.draw(uMaterial, uModelMat, &matModelUniform, &matNormalUniform);
		this->mMap.draw(uMaterial, uModelMat, 35, uIsInstancedRendering, &matModelUniform, &matNormalUniform);

		btShader.bind();
		btCamera.set(this->mWindow.getCamera()->getMatrix());
		v.getControlsDraw().draw(10, 11);

		//draw weather
		weatherShader.bind();
		whCameraMatrix.set(this->mCamera.getMatrix());
		whAmbient.set(this->mAmbient);
		this->mWeatherEffect.draw(whViewMatrix, whColor, 35, 37, this->mCamera, whCamUp, whCamRight, whWeatherCenter);

		shader.bind();

		oit.endTransparentPass(uOITEnabled);

		oit.draw(this->mWindow, sr);

		// gui

		shader.bind();

		if(!hideGui) {
			ImGui::Begin("Frame");

			ImGui::Text("Frame time: %fms / %fus", this->mFrameTimer.getMSfloat(), this->mFrameTimer.getUSfloat());
			ImGui::Text("FPS: %f", 1000.0/this->mFrameTimer.getMSfloat());

			ImGui::End();

			UI::drawLineInfoWindow(this->mLine);
			UI::drawPhysicsInfoWindow(v);

			ImGui::Begin("Visual");

			if(ImGui::SliderFloat("Daylight index",  &this->mAmbient, 0.0, 1.0)) {
				ambientLightStrength.set(this->mAmbient);
				if(this->mAmbient < 0.5) {
					GlobalMaterialStore::setVariant("Material.windowDay", "NIGHT");
					GlobalMaterialStore::setVariant("Material.symbol", "NIGHT");
				}
				else {
					GlobalMaterialStore::resetVariant("Material.windowDay");
					GlobalMaterialStore::resetVariant("Material.symbol");
				}
			}
			if(ImGui::SliderFloat("Sun angle",  &sunAngle, 0.0, 360.0)) {
				rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(sunAngle), glm::vec3(1.0f, 0.0f, 0.0f));

				lightPos = glm::vec3(rotationMatrix * glm::vec4(glm::vec3(0.0f, 0.0f, 20.0f), 1.0f));

				d.direction = glm::normalize(glm::vec4(lightPos, 1.0));
				//mainWindow.getCamera()->moveTo(lightPos);

				ds.setPosDirection(lightPos, -d.direction);

				shader.bind();

				uDirlight.update(&d);
				uDirlight.set();
			}

			//dynamically generate buttons for liveries

			if(ImGui::Button("Change livery to normal")) t3rp.resetVariant("Material.paint");
			auto& liveries = GlobalMaterialStore::getByName("Material.paint")->variantData->variants;
			for(const uint64_t id : liveries) {
				GMSEntry* vr = GlobalMaterialStore::getById(id);
				//cannot add ImGui buttons with same name (need to add ID with NAME##ID e.g. Save##LowerMenu)
				if(ImGui::Button(std::string("Change livery to ").append(vr->variant).c_str())) t3rp.setVariant(vr->name, vr->variant);
			}

			ImGui::End();

			ImGui::Begin("Seasons");

			if(ImGui::Button("Spring/Summer"))
				this->mLine.setWeather((uint16_t)setSeason((WeatherCondition)this->mLine.getWeather(), WeatherCondition::WEATHER_SEASONS_SPRING));
			if(ImGui::Button("Autumn"))
				this->mLine.setWeather((uint16_t)setSeason((WeatherCondition)this->mLine.getWeather(), WeatherCondition::WEATHER_SEASONS_AUTUMN));
			if(ImGui::Button("Winter"))
				this->mLine.setWeather((uint16_t)setSeason((WeatherCondition)this->mLine.getWeather(), WeatherCondition::WEATHER_SEASONS_WINTER));
			if(ImGui::Button("Clear"))
				this->mLine.setWeather((uint16_t)::setWeather((WeatherCondition)this->mLine.getWeather(), WeatherCondition::WEATHER_CLEAR)); //Application class has method with same name
			if(ImGui::Button("Fog"))
				this->mLine.setWeather((uint16_t)::setWeather((WeatherCondition)this->mLine.getWeather(), WeatherCondition::WEATHER_FOG));
			if(ImGui::Button("Rain"))
				this->mLine.setWeather((uint16_t)::setWeather((WeatherCondition)this->mLine.getWeather(), WeatherCondition::WEATHER_RAIN));
			if(ImGui::Button("Snow"))
				this->mLine.setWeather((uint16_t)::setWeather((WeatherCondition)this->mLine.getWeather(), WeatherCondition::WEATHER_SNOW));
			if(ImGui::Button("Lightning"))
				this->mLine.setWeather((uint16_t)::setWeather((WeatherCondition)this->mLine.getWeather(), WeatherCondition::WEATHER_LIGHTING));

			ImGui::End();

			ImGui::Begin("Controls");

			if(ImGui::Button("EMERGENCY BRAKE!!!")) {
				v.getVehicleControlData()->brakeEmergency = !v.getVehicleControlData()->brakeEmergency;
				std::cout << "BRAKE " << v.getVehicleControlData()->brakeEmergency << '\n';
			}
			if(ImGui::Button("Bell")) {
				v.getSoundSimulation()->toggleBell();
			}
			if(ImGui::Button("Info")) {
				v.getSoundSimulation()->toggleInfo();
			}
			if(ImGui::Button("Sander")) {
				v.getVehicleControlData()->sander = !v.getVehicleControlData()->sander;
			}

			if(ImGui::Button("Next announcement")) {
				v.confirmAnnouncement();
				this->mLine.playCurrentAnnouncement();
			}

			ImGui::End();
		}

		return true;
	});

	float vehicleConsumed = v.getEnergyUsed()/3600000; //from joule to kwh
	float vehicleTravelled = v.getDistanceTravelled()/1000; //km

	//calculate amount of emissions
	constexpr float EMISSISIONS_PER_KWH = 0.450; //in kg
	constexpr float EMISSISIONS_CAR_KM = 0.400; //in kg

	float tramEmissions = EMISSISIONS_PER_KWH*vehicleConsumed;
	//5 is capacity of typical car, 1.25 is approx. average
	float carEmissionsMin = v.getMaxPassengersAmount()/5.0*EMISSISIONS_CAR_KM*vehicleTravelled;
	float carEmissionsMax = v.getMaxPassengersAmount()/1.25*EMISSISIONS_CAR_KM*vehicleTravelled;

	//calculate price of electricity
	constexpr float PRICE_EUR_KWH_CZECHIA = 0.35;
	constexpr float PRICE_EUR_KWH_GERMANY = 0.4;
	constexpr float PRICE_EUR_KWH_POLSKA = 0.21;

	float priceEurCzechia = vehicleConsumed*PRICE_EUR_KWH_CZECHIA;
	float priceEurGermany = vehicleConsumed*PRICE_EUR_KWH_GERMANY;
	float priceEurPolska = vehicleConsumed*PRICE_EUR_KWH_POLSKA;

	//convert to local currency

	float priceCzechia = priceEurCzechia*24.980;
	//conversion via CZK (EUR -> CZK -> PLN / EUR*CZK/PLN)
	float pricePolska = priceEurPolska*24.980/5.950;

	float gradePercentage = v.getActualPoints()/(float)v.getMaxPoints();

	std::cout << LogLevel::SUCCESS <<
	"Used energy: " << vehicleConsumed << "kWh\n" <<
	"Travelled distance: " << vehicleTravelled << "km\n" <<
	"--\n" <<
	"Total emissions (Czech electricity generation): " << tramEmissions << "kgCO2e\n" <<
	"Equivalent car emissions (avg. 1.25 people/car): " << carEmissionsMax << "kgCO2e\n" <<
	"Equivalent car emissions (carsharing, 5 people/car): " << carEmissionsMin << "kgCO2e\n" <<
	"--\n" <<
	"Price of electricity for tram in Czechia:" << priceEurCzechia << "EUR (" << priceCzechia << " CZK)\n" <<
	"Price of electricity for tram in Germany:" << priceEurGermany << "EUR\n" <<
	"Price of electricity for tram in Poland:" << priceEurPolska << "EUR (" << pricePolska << " PLN)\n" <<
	"--\n" <<
	"Achieved points: " << v.getActualPoints() << " out of " << v.getMaxPoints() << '\n' <<
	"Percentage: " << gradePercentage*100 << "% (grade " << v.getPointGrade() << ")\n" <<
	LogLevel::RESET;

	return true;
}
