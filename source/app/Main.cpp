#define STB_IMAGE_IMPLEMENTATION
#include "base/Base.hpp"

static float CameraXOffset = 0.0f;
static float CameraYOffset = 0.0f;
static float CameraZOffset = 0.0f;

void KeyEsc(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) {
	aWindow->close();
}

void KeyWASDRFQ(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) {
	if(aAction == GLFW_RELEASE) return;

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
	}
}

const static double FOV = 45.0;

static bool FirstMove = true;
static double LastX = 400, LastY = 400;

static double Pitch = 0.0;
static double Yaw = -90.0; //so we start oriented correctly: 0 is to the right of X axis, 90 is back

glm::vec3 Direction;

void MouseCallback(Window* aWindow, double aX, double aY) {
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
    Window MainWindow("Eurotram", 600, 600, false, true);
	uint32_t MouseCallbackHandle = MainWindow.registerMouseCallback(MouseCallback);
	MainWindow.registerKeyCallback(GLFW_KEY_ESCAPE, KeyEsc);
	uint32_t GenericKeyHandle = MainWindow.registerGenericKeyCallback(KeyWASDRFQ);

	/*
	//data 1: cube with only 2 faces textured
    GLfloat vertices[] = {
		//upper square
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

		//lower square
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f
    };
    GLuint indices[] = {
		0, 1, 2, 2, 3, 0, //top face
		4, 5, 6, 6, 7, 4, //bottom face

		1, 2, 5, 5, 6, 2,  //back face
		0, 3, 4, 4, 7, 3, //front face

		0, 1, 4, 4, 5, 1, //left face
		2, 3, 6, 6, 7, 3 //right face
    };
	*/

	//data 2
	//flip X on opposite faces! (mirror of mirror is no mirror)
	GLfloat vertices[] = {
		//upper square
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

		//lower square
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

		//front
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f,

		//back
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

		//left
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

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

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24 * 5, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const void*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

    GLuint IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 36, indices, GL_STATIC_DRAW);

	const char* vertexsrc =
		"#version 330 core\n"
		"layout(location = 0) in vec3 Position;\n"
		"layout(location = 1) in vec2 TexCoord;\n"
		"uniform mat4 umatrix;\n"
		"out vec2 otexcoord;\n"
		"void main() {\n"
		"	gl_Position = umatrix * vec4(Position, 1.0);\n"
		"	otexcoord = TexCoord;\n"
		"}\n"
	;
	const char* fragmentsrc =
		"#version 330 core\n"
		"out vec4 Color;\n"
		"in vec2 otexcoord;\n"
		"uniform sampler2D tex;\n"
		"void main() {\n"
		"	Color = texture(tex, otexcoord);\n"
		"}\n"
	;

    GLuint shader = glCreateProgram();
    GLuint vertexshader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexshader, 1, &vertexsrc, NULL);
	glCompileShader(vertexshader);
	glShaderSource(fragmentshader, 1, &fragmentsrc, NULL);
	glCompileShader(fragmentshader);

	glAttachShader(shader, vertexshader);
	glAttachShader(shader, fragmentshader);

	glLinkProgram(shader);
	glValidateProgram(shader);

	glDetachShader(shader, vertexshader);
	glDetachShader(shader, fragmentshader);
	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);

	glUseProgram(shader);

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
	GLint matuniform = glGetUniformLocation(shader, "umatrix");

	glBindVertexArray(VAO);

    while (MainWindow.isOpen()) {
        MainWindow.clear();

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

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

		//second cube

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, glm::radians(20.0f*(float)glfwGetTime()), glm::vec3(0.5f, 1.0f, 0.0f));

		matrix = proj * view * model;
		glUniformMatrix4fv(matuniform, 1, GL_FALSE, glm::value_ptr(matrix));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        MainWindow.update();
    }

    stbi_image_free(data);
    glfwTerminate();
    return 0;
}
