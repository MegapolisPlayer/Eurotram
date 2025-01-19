#define MINIAUDIO_IMPLEMENTATION
#include "AudioManager.hpp"

static ma_engine sEngine;
static bool sInitialized = false;

void initAudioEngine() noexcept  {
	if(sInitialized) {
		std::cerr << LogLevel::WARNING << "Audio engine already initialized!" << LogLevel::RESET;
		return;
	}

	ma_result result = ma_engine_init(NULL, &sEngine);
	if(result != MA_SUCCESS) {
		std::cerr << LogLevel::ERROR << "Audio engine initialization failed!" << LogLevel::RESET;
		return;
	}
	sInitialized = true;
}
ma_engine* getAudioEngine() noexcept {
	return &sEngine;
}
void terminateAudioEngine() noexcept {
	if(sInitialized) {
		ma_engine_uninit(&sEngine);
		sInitialized = false;
		return;
	}

	std::cerr << LogLevel::WARNING << "Audio engine already terminated!" << LogLevel::RESET;
}

uint64_t getSoundLengthMs(ma_sound* aSound) noexcept {
	ma_uint64 soundPCMLength = 0;
	ma_sound_get_length_in_pcm_frames(aSound, &soundPCMLength);
	return soundPCMLength / (ma_engine_get_sample_rate(&sEngine) * 1000); //does truncate
}
