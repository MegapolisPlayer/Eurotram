#include "model/Model.hpp"
#include "audio/Audio.hpp"

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
			aWindow->getCamera()->moveTo({10.0f, 10.0f, 10.0f});
			break;
		case GLFW_KEY_X:
			aWindow->close();
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
	static_assert(sizeof(glm::vec3) == sizeof(GLfloat)*3, "Test failed: glm::vec3 has wrong size!");
	setUTF8Encoding();

	initAudioEngine();

	{
		Annunciator a("Linka13.etanc");
		a.setVolume(0.7);
		a.playAnnouncementNext("RADH", true);
		a.playAnnouncementCurrent("RADH", true);
		a.playAnnouncementNext("MUZM", true);
		a.playAnnouncementCurrent("MUZM", true);
		a.playAnnouncementLineChange("MUZM", 23, "ZVON", true);
		a.playAnnouncementNext("IPPV", true);
		a.playAnnouncementCurrent("IPPV", true);
		a.playAnnouncementNext("TEST", true);
		a.playAnnouncementCurrent("TEST", true);
		a.playAnnouncementTerminus(true);
		a.playAnnouncementStart(13, "OLSH", true);
	}

	terminateAudioEngine();
	return 0;

	std::array<float, 4> daylightColor = {100.0f/255.0f, 158.0f/255.0f, 233.0f/255.0f, 1.0f};
	float daylightIndex = 1.0f;

    Window mainWindow("Eurotram", 1000, 1000, false, true);
	mainWindow.setBackgroundColor(daylightColor);

	Camera windowCamera(&mainWindow, glm::vec3(0.0f, 5.0f, 0.0f), 45.0f, 10000.0f, 0.05f);

	uint32_t mouseCallbackHandle = mainWindow.registerMouseCallback(MouseCallback);
	mainWindow.registerKeyCallback(GLFW_KEY_ESCAPE, KeyEsc);
	uint32_t genericKeyHandle = mainWindow.registerGenericKeyCallback(KeyWASDRFQ);
	uint32_t mouseClickHandle = mainWindow.registerClickCallback(MouseClick);
	mainWindow.hideCursor();

	Shader lss("shader/shadowVertex.glsl", "shader/shadowFragment.glsl");
	LightShadow ls(lss);
	UniformMat4 lssMatModel(&lss, 70);

	Shader shader("shader/vertex.glsl", "shader/fragment.glsl");
	shader.bind();

	UniformMat4 matCameraUniform(&shader, 10);
	matCameraUniform.set(mainWindow.getCamera()->getMatrix());
	UniformVec3 cameraPosUniform(&shader, 210);
	UniformMat4 matModelUniform(&shader, 11);
	UniformMat3 matNormalUniform(&shader, 12);
	UniformMat4 matLightUniform(&shader, 13);

	Dirlight d;
	d.color = {208.0f/255.0f, 128.0f/255.0f, 0.0f, 1.0f};
	glm::mat4 rotationMatrix = glm::mat4(1.0);
	d.direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);

	UniformDirlight uDirlight(51);
	uDirlight.update(&d);
	uDirlight.set();

	Pointlight p;
	p.color = {1.0f, 0.0f, 1.0f, 1.0f};
	p.position = {0.0f, 5.0f, 0.0f, 0.0f};
	setAttenuation(AttenuationValues::DISTANCE_100, &p.constant, &p.linear, &p.quadratic);

	Spotlight s;
	s.color = {1.0f, 1.0f, 1.0f, 1.0f};

	setAttenuation(AttenuationValues::DISTANCE_100, &s.constant, &s.linear, &s.quadratic);
	setSpotlightRadius(&s, 15.0, 10.0);

	UniformPointlight uPoints(52, 0);
	uPoints.update(&p);
	uPoints.set();

	UniformSpotlight uSpots(53, 1);
	uSpots.update(&s);
	uSpots.set();

	UniformFloat ambientLightStrength(&shader, 200);
	ambientLightStrength.set(daylightIndex);

	UniformMaterial uMaterial(50);

	Timer loopTimer;
	Timer guiTimer;
	Timer drawTimer;

	TimerAverage avgFrame;

	float sunAngle = 0;

	Transform t1;
	//t1.setRotationY(45.0f);

	//Transform t2;
	//t2.setPosition(glm::vec3(2.0f));

	//Transform t3;
	//t3.setScale(0.2f);

	std::cout << "Loading T3R.P model...\n";
	Model t3rp("T3.obj");
	std::cout << "Model loaded!\n";

    while (mainWindow.isOpen()) {
		loopTimer.start();

		glClear(GL_DEPTH_BUFFER_BIT);
		ls.bind(mainWindow, d);
		lssMatModel.set(glm::mat4(1.0f));
		t3rp.draw(uMaterial);
		ls.unbind(mainWindow);

		mainWindow.beginFrame();

		shader.bind();

		s.position = glm::vec4(mainWindow.getCamera()->getPosition(), 1.0);
		s.direction = glm::vec4(mainWindow.getCamera()->getDirection(), 1.0);
		uSpots.update(&s);

		ls.bindMap(1);
		matLightUniform.set(ls.getMatrix());

		matCameraUniform.set(mainWindow.getCamera()->getMatrix());
		cameraPosUniform.set(mainWindow.getCamera()->getPosition());

		matModelUniform.set(t1.getMatrix());
		matNormalUniform.set(t1.getNormalMatrix());

		drawTimer.start();

		t3rp.draw(uMaterial);

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
			d.direction = rotationMatrix * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
			uDirlight.update(&d);
			uDirlight.set();
		}
		if(ImGui::SliderFloat3("Point light position", glm::value_ptr(p.position), -10.0, 10.0)) {
			uPoints.update(&p);
			uPoints.set();
		}

		ImGui::End();

		ImGui::Begin("Frame");

		ImGui::Text("Frame time: %fms / %fus", loopTimer.getMSfloat(), loopTimer.getUSfloat());
		ImGui::Text("FPS: %f", 1000.0/loopTimer.getMSfloat());
		ImGui::Text("Draw time: %fms / %fus", drawTimer.getMSfloat(), drawTimer.getUSfloat());
		ImGui::Text("GUI draw time: %fms / %fus", guiTimer.getMSfloat(), guiTimer.getUSfloat());

		ImGui::End();

		ImGui::Begin("Line");

		ImGui::Text("Line: %d/%d", 13, 2);
		ImGui::Text("Starting stop: %s", "Zvonarka (ZVON)");
		ImGui::Text("Final stop: %s", "Olsanske hrbitovy (OLHR)");
		ImGui::Text("Next stop: %s", "Muzeum (A/C)");
		ImGui::Text("Vehicle no. %d", 8316);
		ImGui::Text("Control points: %s", "IPPV, FLOR");

		ImGui::End();

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

		ImGui::Begin("Announcements");

		if(ImGui::Button("Next announcement")) {
			//TODO next announcement
		};

		ImGui::End();

		guiTimer.end();

        mainWindow.endFrame();
		loopTimer.end(avgFrame);
    }

    std::cout << "Frametime average\n" << avgFrame.getAverageUSfloat() << "us\n";

	terminateAudioEngine();

    return 0;
}

/*
 // flip X on opposite faces! (mirror of mirror is no mirror)
 //posX, posY, posZ, texCoord, texCoord, normalX, normalY, normalZ
 GLfloat vertices[] = {
 //upper square
 -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

//lower square
0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

//front
-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

//back
0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

//left
-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

//right
0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
};
GLuint indices[] = {
0, 1, 2, 2, 3, 0,
4, 5, 6, 6, 7, 4,
8, 9, 10, 10, 11, 8,
12, 13, 14, 14, 15, 12,
16, 17, 18, 18, 19, 16,
20, 21, 22, 22, 23, 20,
};
*/
