#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "model/Model.hpp"
#include "audio/Audio.hpp"
#include "area/Area.hpp"

static bool FirstMove = true;
static double LastX = 400, LastY = 400;

void KeyEsc(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) {
	aWindow->showCursor();
}

void MouseClick(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) {
	if(aKey == GLFW_MOUSE_BUTTON_RIGHT && aAction == GLFW_PRESS) {
		aWindow->hideCursor();
	}
}

static glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 20.0f);
static bool moveWithSpotlight = true;

void KeyWASDRFQ(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) {
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
	}
}

void MouseCallback(Window* aWindow, double aX, double aY) {
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

int main() {
	//TODO move to EurotramInit::mainInit();
	static_assert(sizeof(glm::vec3) == sizeof(GLfloat)*3, "Test failed: glm::vec3 has wrong size!");
	setUTF8Encoding();
	initAudioEngine();

	Annunciator a("Linka13.etanc");
	a.setVolume(0.7);

	Line l;
	l.open("Linka13-test.etscr", &a);

	std::array<float, 4> daylightColor = {100.0f/255.0f, 158.0f/255.0f, 233.0f/255.0f, 1.0f};
	float daylightIndex = 1.0f;

    Window mainWindow("Eurotram", 1000, 1000, false, true);
	mainWindow.setBackgroundColor(daylightColor);
	mainWindow.enableVSync();

	Camera windowCamera(&mainWindow, glm::vec3(0.0f, 5.0f, 0.0f), 45.0f, 10000.0f, 0.05f);

	mainWindow.registerMouseCallback(MouseCallback);
	mainWindow.registerKeyCallback(GLFW_KEY_ESCAPE, KeyEsc);
	mainWindow.registerGenericKeyCallback(KeyWASDRFQ);
	mainWindow.registerClickCallback(MouseClick);
	mainWindow.hideCursor();

	Shader shader("shader/vertex.glsl", "shader/fragment.glsl");
	shader.bind();

	TextureSamplerArray<16> tsa(232);
	tsa.set();

	UniformMat4 matCameraUniform(10);
	matCameraUniform.set(mainWindow.getCamera()->getMatrix());
	UniformVec3 cameraPosUniform(210);
	UniformMat4 matModelUniform(11);
	UniformMat3 matNormalUniform(12);

	Dirlight d;
	d.color = {208.0f/255.0f, 128.0f/255.0f, 0.0f, 1.0f};
	glm::mat4 rotationMatrix = glm::mat4(1.0);
	d.direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);

	DirectionalShadows ds(lightPos, glm::vec3(0.0f), 20.0f, 0.1f, 30.0f);

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

	SpotlightShadows ss(lightPos, s.direction, 0.1, 30.0, *mainWindow.getCamera()->getFOVPointer());
	UniformMat4 fpu(14);
	fpu.set(ss.getProjectionMatrix());

	UniformFloat ambientLightStrength(200);
	ambientLightStrength.set(daylightIndex);

	Timer loopTimer;
	Timer guiTimer;
	Timer drawTimer;

	TimerAverage avgFrame;

	float sunAngle = 0;

	Transform t1;
	//t1.setRotationY(45.0f);

	std::cout << "Loading T3R.P model...\n";
	Model t3rp(std::filesystem::path("./T3.glb"));
	std::cout << "Model loaded!\n";
	t3rp.addVariant("Material.paint", "PaintTexturePID.png", "PID");
	t3rp.addVariant("Material.paint", "PaintTexturePLF.png", "PLF");

	shader.bind();
	UniformMaterial uMaterial(50);
	StructUniform<glm::mat4> uModelMat(40, 0);

	Shader shadowMapProgram("shader/shadowVertex.glsl", "shader/shadowFragment.glsl");
	shadowMapProgram.bind();
	UniformMat4 lpu(90);
	UniformMat4 lmod(91);
	lmod.set(t1.getMatrix());

	uint64_t i = 0;
    while (mainWindow.isOpen()) {
		loopTimer.start();

 		std::cout << "GLFW anim time " << std::fmod(glfwGetTime(), 3.0) << '\n';
		t3rp.setAnimation("pantographAction", std::fmod(glfwGetTime(), 3.0));
		t3rp.setAnimation("driverDoorAction", std::fmod(glfwGetTime(), 3.0));

		shadowMapProgram.bind();

		ss.beginPass(mainWindow, lpu);
		lmod.set(t1.getMatrix());
		t3rp.draw(uMaterial, uModelMat);
		ss.endPass(mainWindow);

		ds.beginPass(mainWindow, lpu);
		lmod.set(t1.getMatrix());
		t3rp.draw(uMaterial, uModelMat);
		ds.endPass(mainWindow);

		// main draw

		mainWindow.beginFrame();

		shader.bind();
		spu.set(ds.getProjectionMatrix());
		fpu.set(ss.getProjectionMatrix());

		if(moveWithSpotlight) {
			s.position = glm::vec4(mainWindow.getCamera()->getPosition(), 1.0);
			s.direction = glm::vec4(mainWindow.getCamera()->getDirection(), 1.0);
		}
		uSpots.update(&s);
		ss.setPosDirection(s.position, s.direction);

		matCameraUniform.set(mainWindow.getCamera()->getMatrix());
		cameraPosUniform.set(mainWindow.getCamera()->getPosition());

		matModelUniform.set(t1.getMatrix());
		matNormalUniform.set(t1.getNormalMatrix());

		ss.bindMap(12);
		ds.bindMap(15);
		drawTimer.start();

		t3rp.draw(uMaterial, uModelMat);

		drawTimer.end();

		// gui

		guiTimer.start();

		ImGui::Begin("Settings");

		if(ImGui::SliderFloat("FOV", windowCamera.getFOVPointer(), 30.0, 90.0)) {
			windowCamera.update();
		}
		if(ImGui::SliderFloat("Daylight index",  &daylightIndex, 0.0, 1.0)) {
			mainWindow.setBackgroundColor(daylightColor*daylightIndex);
			ambientLightStrength.set(daylightIndex);
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

		ImGui::Text("Frame time: %fms / %fus", loopTimer.getMSfloat(), loopTimer.getUSfloat());
		ImGui::Text("FPS: %f", 1000.0/loopTimer.getMSfloat());
		ImGui::Text("Draw time: %fms / %fus", drawTimer.getMSfloat(), drawTimer.getUSfloat());
		ImGui::Text("GUI draw time: %fms / %fus", guiTimer.getMSfloat(), guiTimer.getUSfloat());

		ImGui::End();

		UI::drawLineInfoWindow(l);

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

		if(ImGui::Button("Change livery to normal")) {
			t3rp.resetVariant("Material.paint");
		};
		if(ImGui::Button("Change livery to PLF")) {
			t3rp.setVariant("Material.paint", "PLF");
		};
		if(ImGui::Button("Change livery to PID")) {
			t3rp.setVariant("Material.paint", "PID");
		};

		if(ImGui::Button("Open driver door")) {
			t3rp.setAnimation("driverDoorAction", i);
			i++;
			if(i == 80) { i = 0; }
		};
		if(ImGui::Button("Close driver door")) {
			t3rp.setAnimation("driverDoorAction", i);
			if(i == 0) { i = 40; }
			i--;
		};

		ImGui::End();

		ImGui::Begin("Announcements");

		if(ImGui::Button("Next announcement")) {
			l.next(1);
		};

		ImGui::End();

		guiTimer.end();

        mainWindow.endFrame();
		loopTimer.end(avgFrame);
    }

    std::cout << "Frametime average\n" << avgFrame.getAverageUSfloat() << "us\n";

	std::atexit([]() {
		terminateAudioEngine();
	});

    return 0;
}
