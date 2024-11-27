/*
PREFIXES
--------
m Members
p Pointers
a Parameters
s Static variables
c Constants
r Raw Arrays
 */

#ifndef EUROTRAM_BASE_SETUP
#define EUROTRAM_BASE_SETUP

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>

#include <stbi/stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#define FILE_READ_BLOCK_SIZE 8192

#ifdef _WIN32 //Windows-only
	#define WIN32_LEAN_AND_MEAN //save on size
	#include <windows.h> //color changing on windows
	#include <tchar.h>   //console title
#endif

#endif
