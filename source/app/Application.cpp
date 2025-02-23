#include "Application.hpp"

Application::Application() noexcept
	: mWindow("Eurotram", 1280, 720, false, true),
	mCamera(&this->mWindow, glm::vec3(0.0f, 5.0f, 0.0f), 45.0f, 1000.0f, 0.1f),
	mLogo("logo.png", false), mMinuteTime(0) {
	this->mWindow.setBackgroundColor(glm::vec4(0.5f));
	this->mWindow.enableVSync();

	this->mWindow.registerGenericKeyCallback(Application::rawKeyCallback);
	this->mWindow.registerMouseCallback(Application::rawMouseCallback);
	this->mWindow.registerClickCallback(Application::rawClickCallback);

	//defaults TODO change
	this->mAnnunciatorPath = "Linka13.etanc";
	this->mLinePath = "line.etscr";
	this->mMapPath = "Praha.etmap";

	//for not graceful exits
	std::atexit([]() {
		terminateAudioEngine();
	});
}

bool Application::renderMainMenu() noexcept {
	while(this->mWindow.isOpen()) {
		this->mWindow.beginFrame();

		ImGui::SetNextWindowPos(ImVec2(100, 100));
		ImGui::SetNextWindowSize(ImVec2(this->mWindow.getWidth()/3.0, this->mWindow.getHeight()-200));
		ImGui::Begin("Eurotram 0.1");
		ImGui::Text("(c) Martin Bykov 2024-2025");

		ImGui::PushItemWidth(150);
		ImGui::PushItemWidth(100);
		ImGui::Image(
			(ImTextureID)(uint64_t)this->mLogo.getHandle(),
			ImVec2(
				this->mWindow.getWidth()/3.0-100,
				(this->mWindow.getWidth()/3.0-100)*((float)this->mLogo.getHeight()/this->mLogo.getWidth())
			)
		);

		if(ImGui::Button("Start simulator")) {
			if(
				this->mAnnunciatorPath.empty() ||
				this->mLinePath.empty() ||
				this->mMapPath.empty()
			) {
				ImGui::Text("Please input filenames in settings!");
			}
			else {
				ImGui::End();
				this->mWindow.endFrame();
				return true; //go to next step
			}
		}
		if(ImGui::Button("Settings")) {
			ImGui::End();
			this->mWindow.endFrame();
			if(!renderSettingsMenu()) {
				return false; //we exit
			}
			continue; //do not end frame twice
		}
		if(ImGui::Button("Quit")) {
			ImGui::End();
			this->mWindow.endFrame();
			return false;
		}

		ImGui::End();

		this->mWindow.endFrame();
	}

	return false;
}
bool Application::renderSettingsMenu() noexcept {
	while(this->mWindow.isOpen()) {
		this->mWindow.beginFrame();

		ImGui::SetNextWindowPos(ImVec2(100, 100));
		ImGui::SetNextWindowSize(ImVec2(this->mWindow.getWidth()-200, this->mWindow.getHeight()-200));
		ImGui::Begin("Settings");
		ImGui::Text("These settings can be changed ingame later.");

		ImGui::InputText("Annunciator", &this->mAnnunciatorPath);
		ImGui::InputText("Line", &this->mLinePath);
		ImGui::InputText("Map", &this->mMapPath);

		ImGui::Checkbox("Show HUD?", &this->mSettings.showHUD);
		ImGui::SliderFloat("FOV", &this->mSettings.FOV, 30.0, 90.0);
		ImGui::Checkbox("Draw debug points?", &this->mSettings.drawPoints);

		if(ImGui::Button("Done")) {
			ImGui::End();
			this->mWindow.endFrame();
			return true;
		}

		ImGui::End();

		this->mWindow.endFrame();
	}

	return false;
}

void Application::togglePauseMenu() noexcept {
	this->mPauseEnabled = !this->mPauseEnabled;
}
void Application::renderPauseMenu() noexcept {

}

void Application::run() noexcept {
	static_assert(sizeof(glm::vec3) == sizeof(GLfloat)*3, "Test failed: glm::vec3 has wrong size!");
	assert(Texture::getAmountOfSlots() >= 32);
	std::cout << "Texture slots available: " << Texture::getAmountOfSlots() << '\n';

	setUTF8Encoding();
	initAudioEngine();

	if(!renderMainMenu()) {
		this->terminate();
		return;
	}

	//TODO loading screen

	this->mAnnunciator.open(this->mAnnunciatorPath);
	this->mAnnunciator.setVolume(0.7);
	this->mLine.open(this->mLinePath, &this->mAnnunciator);
	this->mMap.open(this->mMapPath);

	this->mCamera.setFOV(this->mSettings.FOV);

	this->mWindow.hideCursor();

	if(!this->runInternal()) {
		std::cerr << LogLevel::ERROR << "Application error in preparation phase!\n" << LogLevel::RESET;
	}

	std::cout << "Application frametime average\n" << this->mAverageFrameTime.getAverageUSfloat() << "us\n";
	this->terminate();
}

void Application::runWindowFrame(std::function<bool()> aFunction) noexcept {
	while(this->mWindow.isOpen()) {
		this->mFrameTimer.start();
		this->mWindow.beginFrame();

		this->mCamera.update();

		if(!aFunction()) {
			std::cerr << LogLevel::ERROR << "Application error in main loop!\n" << LogLevel::RESET;
			this->mWindow.endFrame();
			return; //time to exit
		}

		//TODO handle physics updates and time update

		this->mWindow.endFrame();
		this->mFrameTimer.end(this->mAverageFrameTime);
	}
}

void Application::terminate() noexcept {}

Application::~Application() noexcept {}
