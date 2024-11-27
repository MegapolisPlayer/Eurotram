#define STB_IMAGE_IMPLEMENTATION
#include "base/Base.hpp"

static float CameraXOffset = 0.0f;
static float CameraYOffset = 0.0f;
static float CameraZOffset = 0.0f;

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
			CameraZOffset -= 0.05f;
			break;
		case GLFW_KEY_A:
			CameraXOffset -= 0.05f;
			break;
		case GLFW_KEY_S:
			CameraZOffset += 0.05f;
			break;
		case GLFW_KEY_D:
			CameraXOffset += 0.05f;
			break;
		case GLFW_KEY_R:
			CameraYOffset += 0.05f;
			break;
		case GLFW_KEY_F:
			CameraYOffset -= 0.05f;
			break;
		case GLFW_KEY_Q:
			CameraXOffset = 0.0f;
			CameraYOffset = 0.0f;
			CameraZOffset = 0.0f;
			break;
		case GLFW_KEY_X:
			aWindow->close();
			break;
	}
}

const static double FOV = 45.0;

static bool FirstMove = true;
static double LastX = 400, LastY = 400;

static double Pitch = 0.0;
static double Yaw = -90.0; //so we start oriented correctly: 0 is to the right of X axis, 90 is back

glm::vec3 Direction;

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

	Yaw   += DX;
	Pitch += DY;

	//limits
	if(Pitch > 89.0f) {
		Pitch = 89.0f;
	}
	if(Pitch < -89.0f) {
		Pitch = -89.0f;
	}

	Direction = glm::vec3();
	Direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Direction.y = sin(glm::radians(Pitch));
	Direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Direction = glm::normalize(Direction);
}

int main() {
    Window MainWindow("Eurotram", 1000, 1000, false, true);
	uint32_t MouseCallbackHandle = MainWindow.registerMouseCallback(MouseCallback);
	MainWindow.registerKeyCallback(GLFW_KEY_ESCAPE, KeyEsc);
	uint32_t GenericKeyHandle = MainWindow.registerGenericKeyCallback(KeyWASDRFQ);
	uint32_t MouseClickHandle = MainWindow.registerClickCallback(MouseClick);
	MainWindow.hideCursor();

	//data 2
	//flip X on opposite faces! (mirror of mirror is no mirror)
	GLfloat vertices[] = {
		//upper square
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

		//lower square
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

		//front
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f,

		//back
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

		//left
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

		//right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
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
	VertexBuffer vbo(vertices, 24, 5);
	vbo.enableAttribute(&vao, 3);
	vbo.enableAttribute(&vao, 2);
    IndexBuffer ibo(indices, 36);

	Shader shader("shader/vertex.glsl", "shader/fragment.glsl");

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	GLubyte* data = stbi_load("image.jpg", &width, &height, &channels, 4);
	if(!data) {
		std::cerr << "STBI failed to load image!\n";
		return -1;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture);

	glm::vec3 camera_origin = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
	//vector maths - direction of camera
	glm::vec3 camera_direction = glm::normalize(camera_origin - camera_target);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians((float)FOV), 800.0f/800.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::mat4(1.0f);

	glm::mat4 matrix;
	GLint matuniform = glGetUniformLocation(shader.getHandle(), "umatrix");

	vao.bind();

    while (MainWindow.isOpen()) {
        MainWindow.beginFrame();

		//camera
		const float radius = 10.0f;
		//float dz = sin(glfwGetTime()) * radius;
		//float dx = cos(glfwGetTime()) * radius;
		glm::vec3 camera_pos = glm::vec3(CameraXOffset, CameraYOffset, CameraZOffset);
		view = glm::lookAt(camera_pos, camera_pos + Direction, glm::vec3(0.0, 1.0, 0.0));

		//first cube

		model = glm::mat4(1.0f);

		matrix = proj * view * model;
		glUniformMatrix4fv(matuniform, 1, GL_FALSE, glm::value_ptr(matrix));

		ibo.draw();

		//second cube

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, glm::radians(20.0f*(float)glfwGetTime()), glm::vec3(0.5f, 1.0f, 0.0f));

		matrix = proj * view * model;
		glUniformMatrix4fv(matuniform, 1, GL_FALSE, glm::value_ptr(matrix));

		ibo.draw();

		// gui

		ImGui::Begin("Line information");

		ImGui::Text("Line: %d/%d", 13, 2);
		ImGui::Text("Starting stop: %s", "Zvonarka (ZVON)");
		ImGui::Text("Final stop: %s", "Olsanske hrbitovy (OLHR)");
		ImGui::Text("Next stop: %s", "Muzeum (A/C)");
		ImGui::Text("Vehicle no. %d", 8316);
		ImGui::Text("Control points: %s", "IPPV, FLOR");

		ImGui::End();

		ImGui::Begin("Physics information");

		ImGui::Text("Gravitational force (vertical): %f N", 0.0);
		ImGui::Text("Friction force: %f N", 0.0);
		ImGui::Text("Friction cf.: %f", 0.0);
		ImGui::Text("Friction cf. mod.: %s", "none"); //seasons, leaves, ice...
		ImGui::Text("Acceleration force: %f N", 0.0);
		ImGui::Text("Brake force: %f N", 0.0);
		ImGui::Text("Cetripetal force: %f N", 0.0);
		ImGui::Text("L/V ratio: %f ", 0.0);

		ImGui::End();

		ImGui::Begin("Electrical information");

		ImGui::Text("Voltage in network: %f V", 0.0);
		ImGui::Text("Amperage in network: %f A", 0.0);
		ImGui::Text("Contact resistance: %f Ohm", 0.0);
		ImGui::Text("Contact resistance modifiers: %s", "none"); //ice, rain...
		ImGui::Text("Voltage at pantograph: %f V", 0.0);
		ImGui::Text("Amperage at pantograph: %f A", 0.0);
		ImGui::Text("Breakers: %s", "none"); //rain, ice...

		ImGui::End();

        MainWindow.endFrame();
    }

    stbi_image_free(data);
    return 0;
}
