#ifndef EUROTRAM_AUDIO_SFX
#define EUROTRAM_AUDIO_SFX
#include "AudioManager.hpp"

class SoundEffect {
public:
	SoundEffect() noexcept;

	void open(const std::string_view aFilename) noexcept;

	void play() noexcept;
	void stop() noexcept;

	void setVolume(float aVolume) noexcept;

	ma_sound getHandle() const noexcept;

	~SoundEffect() noexcept;
private:
	ma_sound mSound;
	uint64_t mPCMLength;
};

#endif
