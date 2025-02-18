#include "model/Model.hpp"
#include "audio/Audio.hpp"
#include "area/Area.hpp"

//possible settings
struct ApplicationSettings {
	//enable UI elements which are not intergrated into the scenery
	bool showHUD;
	//set field of view
	float FOV;
	//draw points for debug
	bool drawPoints;
};

class Application {
public:
	Application(const bool aFullscreen) noexcept;

	void renderMainMenu() noexcept;
	void renderSettingsMenu() noexcept;

	~Application() noexcept;
private:
	Window mWindow;
	Annunciator mAnnunciator;
	Map mMap;
};
