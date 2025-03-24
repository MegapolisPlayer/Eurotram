#ifndef EUROTRAM_AUDIO_MANAGER
#define EUROTRAM_AUDIO_MANAGER
#include "../base/Base.hpp"

constexpr const char* AUDIO_FILE_EXTENSION = ".wav";

void initAudioEngine() noexcept;
ma_engine* getAudioEngine() noexcept;
void terminateAudioEngine() noexcept;

uint64_t getSoundLengthMs(ma_sound* aSound) noexcept;

#endif
