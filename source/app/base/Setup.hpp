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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <chrono>

#define FILE_READ_BLOCK_SIZE 8192

#ifdef _WIN32 //Win./eurotrdows-only
	#define WIN32_LEAN_AND_MEAN //save on size
	#include <windows.h> //color changing on windows
	#include <tchar.h>   //console title
#endif

//if C++20 or C++23 not defined
#if !(__cplusplus == 202302L || __cplusplus == 202002L)
	#error "C++20 or higher required"
#endif

#endif
