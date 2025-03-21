cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_compile_options(-Wall -Wextra -g -g3 -fno-omit-frame-pointer -fsanitize=address -Wno-unused-parameter -Wno-reorder-ctor) # disable for release mode

project(eurotram VERSION 0.1.0)
add_compile_definitions(GLEW_NO_GLU)
add_executable(eurotram
"app/area/Line.cpp"
"app/area/Map.cpp"

"app/audio/Announcements.cpp"
"app/audio/SFX.cpp"
"app/audio/AudioManager.cpp"
"app/audio/SoundTransform.cpp"

"app/base/Buffer.cpp"
"app/base/Shader.cpp"
"app/base/Texture.cpp"
"app/base/FrustumCulling.cpp"
"app/base/Window.cpp"
"app/base/FreeUtils.cpp"
"app/base/Camera.cpp"
"app/base/Uniform.cpp"
"app/base/Transformation.cpp"
"app/base/Text.cpp"
"app/base/Math.cpp"
"app/base/ScreenRender.cpp"
"app/base/OIT.cpp"

"app/model/Lighting.cpp"
"app/model/Sky.cpp"
"app/model/Material.cpp"
"app/model/ModelLoading.cpp"
"app/model/Mesh.cpp"
"app/model/Animation.cpp"

"app/objects/vehicles/APEX.cpp"
"app/objects/vehicles/BUSE.cpp"
"app/objects/vehicles/Vehicle.cpp"
"app/objects/Building.cpp"
"app/objects/Button.cpp"
"app/objects/Landmark.cpp"
"app/objects/Lightpole.cpp"
"app/objects/Node.cpp"
"app/objects/Radiobox.cpp"
"app/objects/Sign.cpp"
"app/objects/Signal.cpp"
"app/objects/StationPillar.cpp"
"app/objects/Switch.cpp"
"app/objects/SwitchSignal.cpp"
"app/objects/Track.cpp"
"app/objects/Trigger.cpp"
"app/objects/Wire.cpp"

"app/physics/PhysBase.cpp"
"app/physics/Weather.cpp"

"app/ui/HUD.cpp"

"app/ApplicationPhysics.cpp"
"app/ApplicationMainLoop.cpp"
"app/Application.cpp"
"app/Main.cpp"

"depend/glad/src/glad.c"

"depend/imgui/imgui.cpp"
"depend/imgui/imgui_tables.cpp"
"depend/imgui/imgui_widgets.cpp"
"depend/imgui/imgui_draw.cpp"
"depend/imgui/imgui_draw.cpp"
"depend/imgui/backends/imgui_impl_opengl3.cpp"
"depend/imgui/backends/imgui_impl_glfw.cpp"
"depend/imgui/misc/cpp/imgui_stdlib.cpp"

"depend/fastgltf/base64.cpp"
"depend/fastgltf/fastgltf.cpp"
"depend/fastgltf/io.cpp"
"depend/fastgltf/simdjson.cpp"
)

target_include_directories(eurotram PUBLIC
"depend/"
"depend/imgui/"
"depend/imgui/backends/"
"depend/imgui/misc/cpp/"
"depend/fastgltf/"
"depend/glad/include/"
)
target_link_directories(eurotram PUBLIC "depend/")

target_link_libraries(eurotram PUBLIC -lGL -lglfw -fsanitize=address)






