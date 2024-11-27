cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD, 11)
set(CMAKE_C_STANDARD_REQUIRED, TRUE)
set(CMAKE_CXX_STANDARD, 20)
set(CMAKE_CXX_STANDARD_REQUIRED, TRUE)
set(CMAKE_CXX_COMPILER, clang++)

project(eurotram VERSION 0.1.0)
add_compile_definitions(GLEW_NO_GLU)
add_executable(eurotram
"app/audio/Announcements.cpp"
"app/audio/Dispatcher.cpp"
"app/audio/SFX.cpp"

"app/base/Batcher.cpp"
"app/base/Buffer.cpp"
"app/base/Shader.cpp"
"app/base/Texture.cpp"
"app/base/FrustumCulling.cpp"
"app/base/Window.cpp"
"app/base/Lighting.cpp"
"app/base/FreeUtils.cpp"
"app/base/Camera.cpp"
"app/base/Uniform.cpp"

"app/model/ModelLoading.cpp"
"app/model/Mesh.cpp"

"app/objects/Object.cpp"

"app/physics/Force.cpp"
"app/physics/Friction.cpp"
"app/physics/Simulation.cpp"
"app/physics/ElectricalCircuit.cpp"
"app/physics/Weather.cpp"

"app/track/Bezier.cpp"
"app/track/TrackGenerator.cpp"
"app/track/TrackLoader.cpp"

"app/ui/HUD.cpp"
"app/ui/MainMenu.cpp"

"app/Main.cpp"

"depend/imgui/imgui.cpp"
"depend/imgui/imgui_tables.cpp"
"depend/imgui/imgui_widgets.cpp"
"depend/imgui/imgui_draw.cpp"
"depend/imgui/imgui_draw.cpp"
"depend/imgui/backends/imgui_impl_opengl3.cpp"
"depend/imgui/backends/imgui_impl_glfw.cpp"
)
target_include_directories(eurotram PUBLIC
"depend/"
"depend/imgui/"
"depend/imgui/backends/"
)
target_link_directories(eurotram PUBLIC "depend/")
target_link_libraries(eurotram PUBLIC -lGL -lglfw -lGLEW)






