#include "model/Model.hpp"
#include "audio/Audio.hpp"
#include "area/Area.hpp"
#include "objects/vehicles/Vehicle.hpp"
#include "physics/Physics.hpp"

enum struct ApplicationRenderState : uint8_t {
	LOADING_FIRST = 0, //first load
	MAIN_MENU,
	SETTINGS,
	LOADING_GAME,
	MAIN_GAME
};

//possible settings
struct ApplicationSettings {
	//enable UI elements which are not intergrated into the scenery
	bool showHUD = true;
	//set field of view
	float FOV = 45.0;
	//draw points for debug TODO
	bool drawPoints = false;
	//draw trigger boxes TODO
	bool drawTriggers = true;
	//allow vehicle livery change ingame
	bool allowVehicleLiveryChange = true;
};

class Application {
public:
	Application() noexcept;

	//these functions return false when we exit

	//render menu with name, buttons and stuff
	bool renderMainMenu() noexcept;
	//before game - set settings
	bool renderSettingsMenu() noexcept;

	//callbacks
	static void rawKeyCallback(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) noexcept;
	static void rawMouseCallback(Window* aWindow, double aX, double aY) noexcept;
	static void rawClickCallback(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) noexcept;

	static void togglePauseMenu(Window* aWindow, uint32_t aKey, uint32_t aAction, uint32_t aModifiers) noexcept;
	bool renderPauseMenu() noexcept;

	//*this passed as parameter
	//return true if continue, false for exit
	bool runInternal() noexcept;
	bool runPhysics() noexcept;
	void run() noexcept;
	void runWindowFrame(std::function<bool()> aFunction) noexcept;

	void terminate() noexcept;

	void setWeather() noexcept;

	~Application() noexcept;
private:
	ApplicationSettings mSettings;
	static bool mPauseEnabled; //glfw callbacks are static, non-static instance(s) check this when rendering
	static bool mPauseSettings;

	Window mWindow;
	Camera mCamera;

	Texture mLogo; //AFTER WINDOW!

	std::string mAnnunciatorPath;
	Annunciator mAnnunciator;
	std::string mLinePath;
	Line mLine;
	std::string mMapPath;
	Map mMap;

	uint64_t mMinuteTime;

	TimerAverage mAverageFrameTime;
	Timer mFrameTimer;

	double mLastPhysicalUpdateTime;
	double mPhysicalUpdateFreq;

	std::vector<Vehicle> mPlayerVehicles;

	WeatherHandler mWeatherEffect;
	glm::vec4 mSkyColor;
	glm::vec4 mFogColor;
	float mAmbient;

	uint16_t mOldWeather;
};
