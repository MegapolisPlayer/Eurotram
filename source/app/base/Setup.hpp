/*
PREFIXES
--------
m Members
p Pointers
a Parameters
s Static variables
c Private constants (not directly visible by user)
r Raw Arrays

PUBLIC CONSTANTS USE UPPER SNAKE CASE AND NO PREFIX
 */

#ifndef EUROTRAM_BASE_SETUP
#define EUROTRAM_BASE_SETUP

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>

#include <stbi/stb_image.h>
#include <stbi/stb_image_write.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/base64.hpp>
#include <fastgltf/glm_element_traits.hpp>

#include <miniaudio/miniaudio.h>

#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <chrono>
#include <csignal> //extra helpful to raise interrupts when debugging
#include <clocale>
#include <thread>
#include <list>
#include <filesystem>
#include <numeric>

using namespace std::chrono_literals;

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
