#ifndef EUROTRAM_AUDIO_SOUND_TRANSFORM
#define EUROTRAM_AUDIO_SOUND_TRANSFORM
#include "AudioManager.hpp"

class SoundSimulation {
public:
	//run through config again
	SoundSimulation(const std::string_view aConfigFilename) noexcept;

	SoundSimulation(SoundSimulation& aOther) noexcept = delete;
	SoundSimulation& operator=(SoundSimulation& aOther) noexcept = delete;

	SoundSimulation(SoundSimulation&& aOther) noexcept;
	SoundSimulation& operator=(SoundSimulation&& aOther) noexcept;

	void playInfo() noexcept;
	void stopInfo() noexcept;
	void toggleInfo() noexcept;

	void playBell() noexcept;
	void stopBell() noexcept;
	void toggleBell() noexcept;

	void playOpenDoor() noexcept;
	void playCloseDoor() noexcept;
	void playOpenCabinDoor() noexcept;
	void playCloseCabinDoor() noexcept;

	//motor and braking sounds
	//play in physics update function

	//throttle from 0 to 1
	void playMotor(const float aThrottle) noexcept;
	//throttle from 0 to -1
	void playBrake(const float aThrottle) noexcept;

	void playSwitch() noexcept;

	~SoundSimulation() noexcept;
private:
	ma_sound_group* mGroup;

	bool mInfoPlaying;
	ma_sound* mInfo; //znameni k ridici

	bool mBellPlaying;
	ma_sound* mBell;

	ma_sound* mOpenDoor;
	ma_sound* mCloseDoor;
	ma_sound* mOpenCabin;
	ma_sound* mCloseCabin;

	float mMotorThrottle;

	ma_sound* mStart;

	ma_sound* mSwitchPass;

	ma_sound* mBrake;
};

#endif
