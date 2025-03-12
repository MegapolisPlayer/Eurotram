#include "Application.hpp"

static bool FirstMove = true;
static double LastX = 400, LastY = 400;

static glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 20.0f);
static bool moveWithSpotlight = true;

static std::array<float, 4> daylightColor = {100.0f/255.0f, 158.0f/255.0f, 233.0f/255.0f, 1.0f};
static float daylightIndex = 1.0f; //TODO

void Application::rawKeyCallback(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) noexcept {
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
			aWindow->getCamera()->moveTo(lightPos);
			break;
		case GLFW_KEY_X:
			aWindow->close();
			break;
		case GLFW_KEY_P:
			moveWithSpotlight = !moveWithSpotlight;
			break;
		case GLFW_KEY_ESCAPE:
			aWindow->showCursor();
			break;
		case GLFW_KEY_F1:
			//TODO hide gui
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
	this->mWindow.setBackgroundColor(daylightColor);

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

	SpotlightShadows ss(lightPos, s.direction, 1.0, 30.0, *this->mCamera.getFOVPointer());
	UniformMat4 fpu(14);
	fpu.set(ss.getProjectionMatrix());

	UniformFloat ambientLightStrength(200);
	ambientLightStrength.set(daylightIndex);

	float sunAngle = 0;

	std::cout << "Loading T3R.P model...\n";
	//Model t3rp(std::filesystem::path("./untitled.glb"));
	Model t3rp(std::filesystem::path("./T3.glb"));
	std::cout << "Model loaded!\n";
	t3rp.addVariant("Material.paint", "PaintTexturePLF.png", "PLF");
	t3rp.addVariant("Material.paint", "PaintTexturePID.png", "PID");

	//external
	t3rp.addVariant("Material.paint", "vojtovoPID.png", "SPID");
	t3rp.addVariant("Material.paint", "vojtovoDPO.png", "DPO");
	t3rp.addVariant("Material.paint", "adamovoPMDP.png", "PMDP");

	//TODO starting track can be shorter than 6.45 meter

	VehicleInformation t3rpInfo;
	t3rpInfo.model = &t3rp;
	t3rpInfo.bogieNames.push_back("T3 bogie 1");
	t3rpInfo.bogieNames.push_back("T3 bogie 2");
	t3rpInfo.bogieShaftSuffixes.push_back(" wheels F");
	t3rpInfo.bogieShaftSuffixes.push_back(" wheels B");
	t3rpInfo.bogieTrackOffset.push_back(0.0);
	t3rpInfo.bogieTrackOffset.push_back(6.45);
	t3rpInfo.bogieCenterOffset.push_back(glm::vec3(0.0, 0.0, 3.225));
	t3rpInfo.bogieCenterOffset.push_back(glm::vec3(0.0, 0.0, -3.225));

	Vehicle v(this->mMap, this->mLine, t3rpInfo);
	v.getVehiclePhysicsData()->speed = 0.1;

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
	WeatherHandler wh(glm::vec3(0), 10000, 0.05, 0.10, glm::vec4(0.0, 0.0, 0.5, 0.5));

	BoxTrigger bt(glm::vec3(0), glm::vec3(5, 2, 5), 45);
	bt.setColor(glm::vec4(0.0, 0.5, 0.0, 0.5));
	BoxTrigger bt2(glm::vec3(3.6, 0, 3.6), glm::vec3(5, 2, 5), 45.0);
	bt2.setColor(glm::vec4(0.5, 0.0, 0.0, 0.5));

	std::cout << bt.collision(bt2) << '\n';

	Shader btShader("shader/btVertex.glsl", "shader/btFragment.glsl");
	UniformMat4 btCamera(30);

	BoxTriggerDrawer btd;
	btd.add(bt);
	btd.add(bt2);

	this->runWindowFrame([&]() {
		v.update(this->mMap, this->mLine);

		this->mMap.regenerateInstanceArray(toStationCode("ZELV"), toStationCode("OLSH"), toStationCode("FLOR"), toStationCode("RADH"));
		wh.move(glm::vec3(this->mCamera.getPosition().x, 0.0, this->mCamera.getPosition().z));
		wh.advance(0.15);

		shadowMapProgram.bind();
		t3rp.sendAnimationDataToShader(lmat);

		//write to depth buffer
		wh.beginPass(this->mWindow, lpu);
		t3rp.draw(uMaterial, lmat, &lmod);
		this->mMap.draw(uMaterial, lmat, 35, luIsInstancedRendering, &lmod);
		wh.endPass(this->mWindow);

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

		matCameraUniform.set(this->mWindow.getCamera()->getMatrix());
		cameraPosUniform.set(this->mWindow.getCamera()->getPosition());

		ss.bindMap(28); //28 flashlight
		ds.bindMap(31); //31 sun
		//29,30 front lights of tram

		//t3rp.setAnimation("ArmatureAction", std::fmod(glfwGetTime(), 3.3));

		t3rp.setAnimation("pantographAction", std::fmod(glfwGetTime(), 3.3));
		t3rp.setAnimation("driverDoorAction", std::fmod(glfwGetTime(), 3.3));

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
		btd.draw(10, 11);

		//draw weather
		weatherShader.bind();
		whCameraMatrix.set(this->mCamera.getMatrix());
		whAmbient.set(daylightIndex);
		wh.draw(whViewMatrix, whColor, 35, 37, this->mCamera, whCamUp, whCamRight, whWeatherCenter);

		shader.bind();

		oit.endTransparentPass(uOITEnabled);

		oit.draw(this->mWindow, sr);

		// gui

		shader.bind();

		ImGui::Begin("Ingame settings");

		if(ImGui::SliderFloat("Daylight index",  &daylightIndex, 0.0, 1.0)) {
			this->mWindow.setBackgroundColor(daylightColor*daylightIndex);
			ambientLightStrength.set(daylightIndex);
			if(daylightIndex < 0.5) {
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

		ImGui::End();

		ImGui::Begin("Frame");

		ImGui::Text("Frame time: %fms / %fus", this->mFrameTimer.getMSfloat(), this->mFrameTimer.getUSfloat());
		ImGui::Text("FPS: %f", 1000.0/this->mFrameTimer.getMSfloat());

		ImGui::End();

		UI::drawLineInfoWindow(this->mLine);

		ImGui::Begin("Physics");

		ImGui::Text("Gravitational force (vertical): %f N", 0.0);
		ImGui::Text("Friction force: %f N", 0.0);
		ImGui::Text("Friction cf.: %f", 0.0);
		ImGui::Text("Friction cf. mod.: %s", "none"); //seasons, leaves, ice...
		ImGui::Text("Acceleration force: %f N", 0.0);
		ImGui::Text("Brake force: %f N", 0.0);
		ImGui::Text("Cetripetal force: %f N", 0.0);
		ImGui::Text("L/V ratio: %f ", 0.0);

		ImGui::End();

		ImGui::Begin("Electricity");

		ImGui::Text("Voltage in network: %f V", 0.0);
		ImGui::Text("Amperage in network: %f A", 0.0);
		ImGui::Text("Contact resistance: %f Ohm", 0.0);
		ImGui::Text("Contact resistance modifiers: %s", "none"); //ice, rain...
		ImGui::Text("Voltage at pantograph: %f V", 0.0);
		ImGui::Text("Amperage at pantograph: %f A", 0.0);
		ImGui::Text("Breakers: %s", "none"); //rain, ice...

		ImGui::End();

		ImGui::Begin("Visual");

		if(ImGui::Button("Change livery to normal")) t3rp.resetVariant("Material.paint");
		if(ImGui::Button("Change livery to PLF")) t3rp.setVariant("Material.paint", "PLF");
		if(ImGui::Button("Change livery to PID")) t3rp.setVariant("Material.paint", "PID");
		if(ImGui::Button("Change livery to old PID")) t3rp.setVariant("Material.paint", "SPID");
		if(ImGui::Button("Change livery to DPO")) t3rp.setVariant("Material.paint", "DPO");
		if(ImGui::Button("Change livery to PMDP")) t3rp.setVariant("Material.paint", "PMDP");

		ImGui::End();

		ImGui::Begin("Announcements");

		if(ImGui::Button("Next announcement")) {
			this->mLine.next(this->mMinuteTime);
		};

		ImGui::End();

		return true;
	});

	return true;
}
