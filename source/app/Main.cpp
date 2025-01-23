#define STB_IMAGE_WRITE_IMPLEMENTATION
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

static glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 10.0f);

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

	Annunciator a("Linka13.etanc");
	a.setVolume(0.7);

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

	Shader shader("shader/vertex.glsl", "shader/fragment.glsl");
	shader.bind();

	TextureSamplerArray<32> tsa(shader, 232);
	tsa.set();

	UniformMat4 matCameraUniform(shader, 10);
	matCameraUniform.set(mainWindow.getCamera()->getMatrix());
	UniformVec3 cameraPosUniform(shader, 210);
	UniformMat4 matModelUniform(shader, 11);
	UniformMat3 matNormalUniform(shader, 12);

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

	UniformFloat ambientLightStrength(shader, 200);
	ambientLightStrength.set(daylightIndex);

	UniformMaterial uMaterial(50);

	Timer loopTimer;
	Timer guiTimer;
	Timer drawTimer;

	TimerAverage avgFrame;

	float sunAngle = 0;

	Transform t1;
	//t1.setRotationY(45.0f);

	std::cout << "Loading T3R.P model...\n";
	Model t3rp("T3.obj");
	t3rp.addVariant("Material.paint", "PaintTexturePID.png", "PID");
	t3rp.addVariant("Material.paint", "PaintTexturePLF.png", "PLF");
	std::cout << "Model loaded!\n";

	Framebuffer fbo;

	unsigned int shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mainWindow.getWidth(), mainWindow.getHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	fbo.bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	fbo.unbind();

	glm::mat4 orthgonalProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 35.0f);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = orthgonalProjection * lightView;

	Shader shadowMapProgram("shader/shadowVertex.glsl", "shader/shadowFragment.glsl");
	shadowMapProgram.bind();
	UniformMat4 lpu(shadowMapProgram, "lightProjection");
	lpu.set(lightProjection);
	UniformMat4 lmod(shadowMapProgram, "model");
	lmod.set(t1.getMatrix());

	shader.bind();
	UniformMat4 spu(shadowMapProgram, 13);
	spu.set(lightProjection);

	int announcerId = 0;
    while (mainWindow.isOpen()) {
		loopTimer.start();

		shadowMapProgram.bind();

		fbo.bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		t3rp.draw(uMaterial);

		fbo.unbind();

		mainWindow.beginFrame();

		shader.bind();
		spu.set(lightProjection);

		s.position = glm::vec4(mainWindow.getCamera()->getPosition(), 1.0);
		s.direction = glm::vec4(mainWindow.getCamera()->getDirection(), 1.0);
		uSpots.update(&s);

		matCameraUniform.set(mainWindow.getCamera()->getMatrix());
		cameraPosUniform.set(mainWindow.getCamera()->getPosition());

		matModelUniform.set(t1.getMatrix());
		matNormalUniform.set(t1.getNormalMatrix());

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
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

			lightPos = glm::vec3(rotationMatrix * glm::vec4(glm::vec3(0.0f, 0.0f, 20.0f), 1.0f));
			//mainWindow.getCamera()->moveTo(lightPos);

			lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			lightProjection = orthgonalProjection * lightView;

			shadowMapProgram.bind();
			lpu.set(lightProjection);
			shader.bind();

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

		ImGui::End();

		ImGui::Begin("Announcements");

		if(ImGui::Button("Next announcement")) {
			//TODO load line file and play from it
			switch(announcerId) {
				case(0):
					a.playAnnouncementNext("RADH");
					break;
				case(1):
					a.playAnnouncementCurrent("RADH");
					break;
				case(2):
					a.playAnnouncementNext("MUZM");
					break;
				case(3):
					a.playAnnouncementCurrent("MUZM");
					break;
				case(4):
					a.playAnnouncementLineChange("MUZM", 23, "ZVON");
					break;
				case(5):
					a.playAnnouncementNext("IPPV");
					break;
				case(6):
					a.playAnnouncementCurrent("IPPV");
					break;
				case(7):
					a.playAnnouncementNext("TEST");
					break;
				case(8):
					a.playAnnouncementCurrent("TEST");
					break;
				case(9):
					a.playAnnouncementTerminus();
					break;
				case(10):
					a.playAnnouncementStart(13, "OLSH");
					break;
			}
			announcerId++;
			if(announcerId == 11) announcerId = 0;
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
