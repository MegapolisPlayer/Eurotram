#include "SoundSimulation.hpp"

SoundSimulation::SoundSimulation(const std::string_view aConfigFilename) noexcept {
	if(aConfigFilename.empty()) return; //dead object creation

	//miniaudio doesnt work with stack for some reason
	this->mInfoPlaying = false;
	this->mBellPlaying = false;
	this->mMotorThrottle = 0.0;
	this->mBell = nullptr;
	this->mOpenDoor = nullptr;
	this->mCloseDoor = nullptr;
	this->mOpenCabin = nullptr;
	this->mCloseCabin = nullptr;
	this->mStart = nullptr;
	this->mSwitchPass = nullptr;
	this->mBrake = nullptr;

	this->mGroup = new ma_sound_group;
	ma_sound_group_init(getAudioEngine(), 0, NULL, this->mGroup);
	ma_sound_group_set_volume(this->mGroup, 0.7);

	std::ifstream file;
	file.open(aConfigFilename.data(), std::ios::in); //text mode
	if(!file.is_open()) {
		std::cerr << LogLevel::ERROR << "File " << aConfigFilename << " could not be opened!\n" << LogLevel::RESET;
		return;
	}

	std::string buffer;
	while(std::getline(file, buffer)) {
		if(buffer.empty()) continue;
		if(buffer[0] == '#') continue; //comment

		std::string name = buffer.substr(0, buffer.find('='));
		std::string value = buffer.substr(buffer.find('=')+1);

		std::cout << "Sound simulation config: " << value << " is " << name << '\n';

		ma_sound** resultSound = nullptr;
		if(name == "SOUNDOPENDOOR") resultSound = &this->mOpenDoor;
		else if(name == "SOUNDCLOSEDOOR") resultSound = &this->mCloseDoor;
		else if(name == "SOUNDOPENCABIN") resultSound = &this->mOpenCabin;
		else if(name == "SOUNDCLOSECABIN") resultSound = &this->mCloseCabin;
		else if(name == "SOUNDSTART") resultSound = &this->mStart;
		else if(name == "SOUNDBRAKE") resultSound = &this->mBrake;
		else if(name == "SOUNDSWITCH") resultSound = &this->mSwitchPass;
		else if(name == "SOUNDINFO") resultSound = &this->mInfo;
		else if(name == "SOUNDBELL") resultSound = &this->mBell;
		else { continue; }

		value.append(AUDIO_FILE_EXTENSION);
		std::cout << "Loading sound " << value << '\n';

		*resultSound = new ma_sound;

		ma_result result = ma_sound_init_from_file(getAudioEngine(), value.c_str(), 0, this->mGroup, NULL, *resultSound);
		if(result != MA_SUCCESS) {
			std::cerr << LogLevel::ERROR << "Sound " << value << " failed to load: " << ma_result_description(result) << "\n" << LogLevel::RESET;
			return;
		}
	}

	file.close();
}

SoundSimulation::SoundSimulation(SoundSimulation&& aOther) noexcept {
	this->mGroup = aOther.mGroup;
	aOther.mGroup = nullptr;
	this->mInfoPlaying = aOther.mBellPlaying;
	this->mInfo = aOther.mInfo;
	aOther.mInfo = nullptr;
	this->mBellPlaying = aOther.mBellPlaying;
	this->mBell = aOther.mBell;
	aOther.mBell = nullptr;
	this->mOpenDoor = aOther.mOpenDoor;
	aOther.mOpenDoor = nullptr;
	this->mCloseDoor = aOther.mCloseDoor;
	aOther.mCloseDoor = nullptr;
	this->mOpenCabin = aOther.mOpenCabin;
	aOther.mOpenCabin = nullptr;
	this->mCloseCabin = aOther.mCloseCabin;
	aOther.mCloseCabin = nullptr;
	this->mMotorThrottle = aOther.mMotorThrottle;
	this->mStart = aOther.mStart;
	aOther.mStart = nullptr;
	this->mSwitchPass = aOther.mSwitchPass;
	aOther.mSwitchPass = nullptr;
	this->mBrake = aOther.mBrake;
	aOther.mBrake = nullptr;
}
SoundSimulation& SoundSimulation::operator=(SoundSimulation&& aOther) noexcept {
	this->mGroup = aOther.mGroup;
	aOther.mGroup = nullptr;
	this->mInfoPlaying = aOther.mBellPlaying;
	this->mInfo = aOther.mInfo;
	aOther.mInfo = nullptr;
	this->mBellPlaying = aOther.mBellPlaying;
	this->mBell = aOther.mBell;
	aOther.mBell = nullptr;
	this->mOpenDoor = aOther.mOpenDoor;
	aOther.mOpenDoor = nullptr;
	this->mCloseDoor = aOther.mCloseDoor;
	aOther.mCloseDoor = nullptr;
	this->mOpenCabin = aOther.mOpenCabin;
	aOther.mOpenCabin = nullptr;
	this->mCloseCabin = aOther.mCloseCabin;
	aOther.mCloseCabin = nullptr;
	this->mMotorThrottle = aOther.mMotorThrottle;
	this->mStart = aOther.mStart;
	aOther.mStart = nullptr;
	this->mSwitchPass = aOther.mSwitchPass;
	aOther.mSwitchPass = nullptr;
	this->mBrake = aOther.mBrake;
	aOther.mBrake = nullptr;
	return *this;
}

void SoundSimulation::playInfo() noexcept {
	this->mInfoPlaying = true;
	ma_sound_seek_to_pcm_frame(this->mInfo, 0);
	ma_sound_start(this->mInfo);
	ma_sound_set_end_callback(this->mInfo, [](void* aClass, ma_sound* aSound) {
		if(((SoundSimulation*)aClass)->mInfoPlaying) {
			ma_sound_seek_to_pcm_frame(aSound, 0);
			ma_sound_start(aSound);
		}
	}, this);
}
void SoundSimulation::stopInfo() noexcept {
	ma_sound_stop(this->mInfo);
	ma_sound_set_end_callback(this->mInfo, nullptr, nullptr);
	this->mInfoPlaying = false;
}
void SoundSimulation::toggleInfo() noexcept {
	if(this->mInfoPlaying) this->stopInfo();
	else this->playInfo();
}

void SoundSimulation::playBell() noexcept {
	this->mBellPlaying = true;
	ma_sound_seek_to_pcm_frame(this->mBell, 0);
	ma_sound_start(this->mBell);
	ma_sound_set_end_callback(this->mBell, [](void* aClass, ma_sound* aSound) {
		if(((SoundSimulation*)aClass)->mBellPlaying) {
			ma_sound_seek_to_pcm_frame(aSound, 0);
			ma_sound_start(aSound);
		}
	}, this);
}
void SoundSimulation::stopBell() noexcept {
	ma_sound_stop(this->mBell);
	ma_sound_set_end_callback(this->mBell, nullptr, nullptr);
	this->mBellPlaying = false;
}
void SoundSimulation::toggleBell() noexcept {
	if(this->mBellPlaying) this->stopBell();
	else this->playBell();
}

void SoundSimulation::playOpenDoor() noexcept {
	ma_sound_seek_to_pcm_frame(this->mOpenDoor, 0);
	ma_sound_start(this->mOpenDoor);
}
void SoundSimulation::playCloseDoor() noexcept {
	ma_sound_seek_to_pcm_frame(this->mCloseDoor, 0);
	ma_sound_start(this->mCloseDoor);
}
void SoundSimulation::playOpenCabinDoor() noexcept {
	ma_sound_seek_to_pcm_frame(this->mOpenCabin, 0);
	ma_sound_start(this->mOpenCabin);
}
void SoundSimulation::playCloseCabinDoor() noexcept {
	ma_sound_seek_to_pcm_frame(this->mCloseCabin, 0);
	ma_sound_start(this->mCloseCabin);
}

void SoundSimulation::playMotor(const float aThrottle) noexcept {
	if(aThrottle <= 0.0) return;
	if(ma_sound_is_playing(this->mStart)) return;

	ma_uint64 pcmPerSecond = ma_engine_get_sample_rate(getAudioEngine());

	float clampedThrottle = std::clamp(aThrottle, 0.0f, 1.0f);
	ma_uint64 length = 0;
	ma_sound_get_length_in_pcm_frames(this->mStart, &length);
	//200ms repeat
	ma_sound_seek_to_pcm_frame(this->mStart, clampedThrottle*length-(pcmPerSecond*0.2));
	ma_sound_set_stop_time_in_pcm_frames(this->mStart, ma_engine_get_time_in_pcm_frames(getAudioEngine()) + pcmPerSecond*0.2);
	ma_sound_start(this->mStart);
}
void SoundSimulation::playBrake(const float aThrottle) noexcept {
	if(aThrottle >= 0.0) return;
	if(ma_sound_is_playing(this->mBrake)) return;

	ma_uint64 pcmPerSecond = ma_engine_get_sample_rate(getAudioEngine());

	float clampedThrottle = -std::clamp(aThrottle, -1.0f, 0.0f);
	ma_uint64 length = 0;
	ma_sound_get_length_in_pcm_frames(this->mBrake, &length);
	//200ms repeat
	ma_sound_seek_to_pcm_frame(this->mBrake, clampedThrottle*length-(pcmPerSecond*0.2));
	ma_sound_set_stop_time_in_pcm_frames(this->mBrake, ma_engine_get_time_in_pcm_frames(getAudioEngine()) + pcmPerSecond*0.2);
	ma_sound_start(this->mBrake);
}

void SoundSimulation::playSwitch() noexcept {
	ma_sound_seek_to_pcm_frame(this->mSwitchPass, 0);
	ma_sound_start(this->mSwitchPass);
}

SoundSimulation::~SoundSimulation() noexcept {
	if(this->mInfo) ma_sound_uninit(this->mInfo); delete this->mInfo;
	if(this->mBell) ma_sound_uninit(this->mBell); delete this->mBell;
	if(this->mOpenDoor) ma_sound_uninit(this->mOpenDoor); delete this->mOpenDoor;
	if(this->mCloseDoor) ma_sound_uninit(this->mCloseDoor); delete this->mCloseDoor;
	if(this->mOpenCabin) ma_sound_uninit(this->mOpenCabin); delete this->mOpenCabin;
	if(this->mCloseCabin) ma_sound_uninit(this->mCloseCabin); delete this->mCloseCabin;
	if(this->mStart) ma_sound_uninit(this->mStart); delete this->mStart;
	if(this->mSwitchPass) ma_sound_uninit(this->mSwitchPass); delete this->mSwitchPass;
	if(this->mBrake) ma_sound_uninit(this->mBrake); delete this->mBrake;
	if(this->mGroup) ma_sound_group_uninit(this->mGroup); delete this->mGroup;
}
