cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")

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
"app/base/FreeUtils.cpp"
"app/base/Camera.cpp"
"app/base/Uniform.cpp"
"app/base/Transformation.cpp"

"app/model/Lighting.cpp"
"app/model/Sky.cpp"
"app/model/Material.cpp"
"app/model/ModelLoading.cpp"
"app/model/Mesh.cpp"
"app/model/Bones.cpp"

"app/objects/Button.cpp"
"app/objects/Object.cpp"
"app/objects/Station.cpp"
"app/objects/Vehicle.cpp"
"app/objects/Trigger.cpp"
"app/objects/Track.cpp"
"app/objects/vehicles/T3RP.cpp"

"app/physics/Force.cpp"
"app/physics/Friction.cpp"
"app/physics/Simulation.cpp"
"app/physics/ElectricalCircuit.cpp"
"app/physics/Weather.cpp"
"app/physics/FlangeClimbing.cpp"

"app/area/Bezier.cpp"
"app/area/TrackGenerator.cpp"
"app/area/TrackLoader.cpp"
"app/area/Building.cpp"
"app/area/Chunking.cpp"

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
"depend/assimp/"
)
target_link_directories(eurotram PUBLIC "depend/")
target_link_libraries(eurotram PUBLIC -lGL -lglfw -lGLEW -lassimp)






