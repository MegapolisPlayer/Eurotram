#include "model/Model.hpp"

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

	std::array<float, 4> daylightColor = {100.0f/255.0f, 158.0f/255.0f, 233.0f/255.0f, 1.0f};
	float daylightIndex = 1.0f;

    Window mainWindow("Eurotram", 1000, 1000, false, true);
	mainWindow.setBackgroundColor(daylightColor);

	Camera windowCamera(&mainWindow, glm::vec3(0.0f, 0.0f, 5.0f), 45.0f, 100.0, 0.05f);

	uint32_t mouseCallbackHandle = mainWindow.registerMouseCallback(MouseCallback);
	mainWindow.registerKeyCallback(GLFW_KEY_ESCAPE, KeyEsc);
	uint32_t genericKeyHandle = mainWindow.registerGenericKeyCallback(KeyWASDRFQ);
	uint32_t mouseClickHandle = mainWindow.registerClickCallback(MouseClick);
	mainWindow.hideCursor();

	//flip X on opposite faces! (mirror of mirror is no mirror)
	//posX, posY, posZ, texCoord, texCoord, normalX, normalY, normalZ
	GLfloat vertices[] = {
		//upper square
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		//lower square
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,

		//front
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

		//back
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

		//left
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		//right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	};
	GLuint indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20,
	};

	VertexArray vao;
	VertexBuffer vbo(vertices, 24, 8);
	vbo.enableAttribute(&vao, 3); //pos
	vbo.enableAttribute(&vao, 2); //texCoord
	vbo.enableAttribute(&vao, 3); //normals
    IndexBuffer ibo(indices, 36);

	Texture texture("image.jpg", 0);

	Shader shader("shader/vertex.glsl", "shader/fragment.glsl");
	shader.bind();

	UniformMat4 matCameraUniform(&shader, 10);
	matCameraUniform.set(mainWindow.getCamera()->getMatrix());
	UniformVec3 cameraPosUniform(&shader, 110);
	UniformMat4 matModelUniform(&shader, 11);
	UniformMat3 matNormalUniform(&shader, 12);

	Material objectMaterial;
	objectMaterial.specular = 1.0f;
	objectMaterial.shininess = 0.25f;
	objectMaterial.diffuse = 0.5f;
	objectMaterial.textureSlot = 0;
	objectMaterial.textureAmount = 1.0f;
	objectMaterial.color = {0.0f, 0.0f, 1.0f};

	Material lightMaterial;
	lightMaterial.specular = 1.0f;
	lightMaterial.shininess = 1.0f;
	lightMaterial.diffuse = 1.0f;
	lightMaterial.textureSlot = 0;
	lightMaterial.textureAmount = 0.0f;
	lightMaterial.color = {1.0f, 1.0f, 1.0f};
	lightMaterial.brightness = 1.0f;

	UniformFloat ambientLightStrength(&shader, 102);
	ambientLightStrength.set(daylightIndex);

	UniformMaterial uMaterial(0);

	UniformVec3 lightPos(&shader, 100);
	glm::vec3 lightPosVector = {0.0f, 5.0f, 10.0f};
	lightPos.set(lightPosVector);

	UniformVec3 lightColor(&shader, 101);
	lightColor.set({208.0f/255.0f, 128.0f/255.0f, 0.0f});

	vao.bind();

	Timer loopTimer;
	Timer guiTimer;
	Timer drawTimer;
	Timer uniformTimer;

	float sunAngle = 0;

	Transform t1;
	t1.setRotationY(45.0f);

	Transform t2;
	t2.setPosition(glm::vec3(2.0f));

	Transform t3;
	t3.setScale(0.2f);

    while (mainWindow.isOpen()) {
		loopTimer.start();
        mainWindow.beginFrame();

		uniformTimer.start();

		uMaterial.update(&objectMaterial);
		uMaterial.set();

		matCameraUniform.set(mainWindow.getCamera()->getMatrix());
		cameraPosUniform.set(mainWindow.getCamera()->getPosition());

		uniformTimer.end();

		drawTimer.start();

		//first cube

		matModelUniform.set(t1.getMatrix());
		matNormalUniform.set(t1.getNormalMatrix());

		ibo.draw();

		//second cube

		//glm::vec3(0.5f, 1.0f, 0.0f)

		t2.setRotationX(20.0f*(float)glfwGetTime()*0.5f);
		t2.setRotationY(20.0f*(float)glfwGetTime());

		matModelUniform.set(t2.getMatrix());
		matNormalUniform.set(t2.getNormalMatrix());

		ibo.draw();

		//light cube

		sunAngle += 0.0001f;
		if(sunAngle >= 360.0f) {
			sunAngle = 0.0f;
		}

		uMaterial.update(&lightMaterial);
		uMaterial.set();

		lightPosVector = glm::vec3(0.0f, sin(sunAngle)*2.0f, cos(sunAngle)*2.0f);
		lightPos.set(lightPosVector);
		t3.setPosition(lightPosVector);

		matModelUniform.set(t3.getMatrix());
		matNormalUniform.set(t3.getNormalMatrix());

		ibo.draw();

		drawTimer.end();

		// gui

		guiTimer.start();

		ImGui::Begin("Settings");

		ImGui::Text("FOV: %f", *windowCamera.getFOVPointer());
		if(ImGui::SliderFloat("FOV", windowCamera.getFOVPointer(), 30.0, 90.0)) {
			//if changed
			windowCamera.update();
		}

		ImGui::Text("Daylight index: %f", daylightIndex);
		if(ImGui::SliderFloat("Daylight index",  &daylightIndex, 0.0, 1.0)) {
			mainWindow.setBackgroundColor(daylightColor*daylightIndex);
			ambientLightStrength.set(daylightIndex);
		}

		ImGui::End();

		ImGui::Begin("Frame");

		ImGui::Text("Frame time: %fms / %fus", loopTimer.getMSfloat(), loopTimer.getUSfloat());
		ImGui::Text("FPS: %f", 1000.0/loopTimer.getMSfloat());
		ImGui::Text("Draw time: %fms / %fus", drawTimer.getMSfloat(), drawTimer.getUSfloat());
		ImGui::Text("GUI draw time: %fms / %fus", guiTimer.getMSfloat(), guiTimer.getUSfloat());
		ImGui::Text("Uniform setup time: %fms / %fus", uniformTimer.getMSfloat(), uniformTimer.getUSfloat());

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

		guiTimer.end();

        mainWindow.endFrame();
		loopTimer.end();
    }
    return 0;
}
