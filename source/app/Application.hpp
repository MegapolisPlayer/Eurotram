#include "model/Model.hpp"
#include "audio/Audio.hpp"
#include "area/Area.hpp"

class Application {
public:
	Application(const bool aFullscreen) noexcept;

	~Application() noexcept;
private:
	Window mWindow;
	Annunciator mAnnunciator;
	Map mMap;
};
