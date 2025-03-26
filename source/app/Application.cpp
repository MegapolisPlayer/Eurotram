#include "Application.hpp"
#include "physics/Weather.hpp"

bool Application::mPauseEnabled = false;
bool Application::mPauseSettings = false;

Application::Application() noexcept
	: mWindow("Eurotram", 1280, 720, false, true),
	mCamera(&this->mWindow, glm::vec3(0.0f, 5.0f, 0.0f), 45.0f, 1000.0f, 0.1f),
	mLogo("logo.png", false), mMinuteTime(0), mPhysicalUpdateFreq(1.0/50.0), mLastPhysicalUpdateTime(0.0),
	mWeatherEffect(this->mCamera.getPosition(), 1000, 0.05, 0.05, glm::vec4(1, 0, 1, 1), 0.15) {
	this->mWindow.setBackgroundColor(glm::vec4(0.5f));
	this->mWindow.enableVSync();

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

		//an admission of guilt... (this took approx me 3 months)
		ImGui::TextWrapped(
			"This is a very early prototype that still has a lot of issues.\n"
			"Consider this a tech demo - a quickly patched-together demonstration program.\n"
			"         -M.B. 31.3.2025"
		);

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

void Application::togglePauseMenu(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) noexcept {
	if(aAction != GLFW_PRESS) return;

	Application::mPauseEnabled = !Application::mPauseEnabled;

	//force show cursor so no softlock
	if(Application::mPauseEnabled) {
		aWindow->showCursor();
		aWindow->disableCallbacks();
	}
	else {
		aWindow->hideCursor();
		aWindow->enableCallbacks();
	}
}
bool Application::renderPauseMenu() noexcept {
	if(Application::mPauseEnabled) {
		if(Application::mPauseSettings) {
			if(!Application::renderSettingsMenu()) {
				//returns true on done button press
				Application::mPauseSettings = false;
			}
			return true;
		}
		//else not required here
		ImGui::SetNextWindowPos(ImVec2(this->mWindow.getWidth()/6.0*2.0, 100));
		ImGui::SetNextWindowSize(ImVec2(this->mWindow.getWidth()/3.0, this->mWindow.getHeight()-200));
		ImGui::Begin("Pause menu");

		if(ImGui::Button("Continue")) {
			Application::togglePauseMenu(&this->mWindow, GLFW_KEY_ESCAPE, GLFW_PRESS, 0);
			ImGui::End();
			return true;
		}

		ImGui::Checkbox("Show HUD?", &this->mSettings.showHUD);
		ImGui::SliderFloat("FOV", &this->mSettings.FOV, 30.0, 90.0);
		ImGui::Checkbox("Draw debug points?", &this->mSettings.drawPoints);

		if(ImGui::Button("Quit")) {
			ImGui::End();
			return false;
		}

		ImGui::End();
	}

	//update settings!
	this->mCamera.setFOV(this->mSettings.FOV);

	return true;
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

	//register callbacks AFTER main menu
	this->mWindow.registerGenericKeyCallback(Application::rawKeyCallback);
	this->mWindow.registerMouseCallback(Application::rawMouseCallback);
	this->mWindow.registerClickCallback(Application::rawClickCallback);

	this->mWindow.registerKeyCallback(GLFW_KEY_ESCAPE, Application::togglePauseMenu);

	//TODO loading screen

	this->mAnnunciator.open(this->mAnnunciatorPath);
	this->mAnnunciator.setVolume(0.7);
	this->mLine.open(this->mLinePath, &this->mAnnunciator);
	this->mMap.open(this->mMapPath);
	initSeasonMaterials(this->mMap);

	this->mCamera.setFOV(this->mSettings.FOV);

	this->mSkyColor = {100.0f/255.0f, 158.0f/255.0f, 233.0f/255.0f, 1.0f};
	if(Math::getRandomNumber(0, 1000) <= 1.1) {
		//sandstorm
		this->mFogColor = {117.0f/255.0f, 100.0f/255.0f, 58.0f/255.0f, 1.0f};
	}
	else {
		this->mFogColor = {0.5f, 0.5f, 0.5f, 1.0f};

	}
	this->mAmbient = 1.0;

	if(this->mLine.getWeather() & (uint16_t)WeatherCondition::WEATHER_FOG) initLightningHandler(this->mFogColor*this->mAmbient);
	else initLightningHandler(this->mSkyColor*this->mAmbient);

	this->setWeather();

	this->mWindow.hideCursor();

	glfwSetTime(0.0); //start count here

	if(!this->runInternal()) {
		std::cerr << LogLevel::ERROR << "Application error in preparation phase!\n" << LogLevel::RESET;
	}

	this->terminate();
}

void Application::runWindowFrame(std::function<bool()> aFunction) noexcept {
	while(this->mWindow.isOpen()) {
		this->mFrameTimer.start();
		this->mWindow.beginFrame();

		if(this->mLine.getWeather() & (uint16_t)WeatherCondition::WEATHER_FOG) this->mWindow.setBackgroundColor(this->mFogColor*this->mAmbient);
		else this->mWindow.setBackgroundColor(this->mSkyColor*this->mAmbient);
		this->mWindow.applyBackgroundColor();

		lightningHandler(this->mWindow, &this->mAmbient, (WeatherCondition)this->mLine.getWeather());

		if(this->mLine.getWeather() != this->mOldWeather) {
			this->mOldWeather = this->mLine.getWeather();
			this->setWeather();
		}

		this->mCamera.update();

		if(!aFunction()) {
			std::cerr << LogLevel::ERROR << "Application error in main loop!\n" << LogLevel::RESET;
			this->mWindow.endFrame();
			return; //time to exit
		}

		double currentTime = glfwGetTime();

		if(currentTime-mLastPhysicalUpdateTime >= mPhysicalUpdateFreq) {
			mLastPhysicalUpdateTime = currentTime;
			if(!this->runPhysics()) {
				std::cerr << LogLevel::ERROR << "Application error in physics loop!\n" << LogLevel::RESET;
				this->mWindow.endFrame();
				return;
			}
		}

		//render pause menu
		if(!this->renderPauseMenu()) {
			//if false - quit button pressed
			this->mWindow.endFrame();
			return;
		}

		this->mWindow.endFrame();
		this->mFrameTimer.end(this->mAverageFrameTime);
	}
}

void Application::terminate() noexcept {
	std::cout << "Application frametime average\n" << this->mAverageFrameTime.getAverageUSfloat() << "us\n";
}

void Application::setWeather() noexcept {
	setSeasonMaterials((WeatherCondition)this->mLine.getWeather());

	if(this->mLine.getWeather() & (uint16_t)WeatherCondition::WEATHER_RAIN || this->mLine.getWeather() & (uint16_t)WeatherCondition::WEATHER_LIGHTING) {
		this->mWeatherEffect = WeatherHandler(this->mCamera.getPosition(), 5000, 0.05, 0.10, glm::vec4(0.0, 0.0, 0.5, 0.5), 0.15);
	}
	else if(this->mLine.getWeather() & (uint16_t)WeatherCondition::WEATHER_SNOW) {
		this->mWeatherEffect = WeatherHandler(this->mCamera.getPosition(), 5000, 0.05, 0.05, glm::vec4(1.0, 1.0, 1.5, 0.5), 0.03);
	}
	else {
		this->mWeatherEffect = WeatherHandler(this->mCamera.getPosition(), 0, 0, 0, glm::vec4(0.0), 0); //dead object
	}

	//normal is 1000, fog sets to 50
	if(this->mLine.getWeather() & (uint16_t)WeatherCondition::WEATHER_FOG) {
		this->mCamera.setFarPlane(50.0f);
	}
	else {
		this->mCamera.setFarPlane(1000.0f);
	}
}

Application::~Application() noexcept {}
