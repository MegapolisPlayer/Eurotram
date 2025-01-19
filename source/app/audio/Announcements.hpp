#ifndef EUROTRAM_AUDIO_ANNOUNCEMENTS
#define EUROTRAM_AUDIO_ANNOUNCEMENTS
#include "AudioManager.hpp"

constexpr uint64_t ANNUNCIATOR_FILE_FORMAT_VERSION = 0;

enum class AnnunciatorBaseSound : uint8_t {
	JINGLE = 0,
	NEXT_STOP,
	NEXT_STOP_ON_REQUEST,
	THIS_STOP_ON_REQUEST,
	EXIT_TO_ROAD,
	TERMINUS,
	TERMINUS_OTHER_LANGUAGE,
	TRANSFER_TO_METRO,
	TRANSFER_TO_RAIL,
	LINE_ROUTE_CHANGE,
	WARNING_FOR_PASSENGERS,
	THE_METRO_STATION_IS,
	CURRENTLY_CLOSED,
	GOING_TO_DEPOT,
	FROM_THE_STATION,
	CONTINUES_JOURNEY_AS,
	DIRECTION,
	FUNICULAR_CLOSED,
	AMOUNT_SOUNDS, //not real sound, just says how many sounds are there
};

enum AnnunciatorStationFlags : uint8_t {
	STATION_FLAG_NONE              = 0b00000000,
	STATION_FLAG_METRO_TRANSFER    = 0b00000001,
	STATION_FLAG_TRAIN_TRANSFER    = 0b00000010,
	STATION_FLAG_FUNICULAR_CLOSURE = 0b00000100,
	STATION_FLAG_ON_REQUEST        = 0b00001000,
};

//IMPORTANT - miniaudio structs ARE NOT COPIABLE OR MOVABLE BY DEFAULT
//you need to use ma_sound_init_copy
//TODO later make wrapper around miniaudio with modern C++ with copying and classes!!!!

struct AnnunciatorStationSound {
	ma_sound* sound;
	uint32_t stationCode; //always <= 4 chars, represented as 32 bit int
	std::string stationName; //UTF-8, at least should be utf-8
	std::vector<uint8_t> lines; //lines which go through station
	std::vector<std::string> letteredLines; //non-numbered lines which go through station
	uint8_t flags;
};

struct AnnunciatorLineSound {
	ma_sound* sound;
	uint8_t lineNumber;
	std::string lineName; //if lineNumber is 0 this is used instead - e.g. for lines such as XC...
};

class Annunciator {
public:
	//just initializes class - call open() later
	Annunciator() noexcept;
	//loads a .etanc file - extension required!
	Annunciator(const std::string_view aFilename) noexcept;

	void open(const std::string_view aFilename) noexcept;

	void playBaseAnnouncement(const AnnunciatorBaseSound aSound, const bool aWaitUntilDone = false) noexcept;

	void playAnnouncementCurrent(const std::string_view aStationCode, const bool aWaitUntilDone = false) noexcept;
	void playAnnouncementNext(const std::string_view aNextStationCode, const bool aWaitUntilDone = false) noexcept;
	void playAnnouncement(const std::string_view aStationCode, const std::string_view aNextStationCode, const bool aWaitUntilDone = false) noexcept;

	void playAnnouncementLineChange(const std::string_view aStationCode, const uint8_t aNewLine, const std::string_view aNewEndStationCode, const bool aWaitUntilDone = false) noexcept;
	void playAnnouncementTerminus(const bool aWaitUntilDone = false); //only plays the terminus announcement - not the station one

	//TODO add checkbox if metro station closed!

	void setVolume(const float aVolume) noexcept;

	void destroy() noexcept;

	//returns empty string on fail

	std::string_view getStationName(const std::string_view aStationCode) const noexcept;
	std::string_view getStationName(const uint32_t aStationCode) const noexcept;

	~Annunciator() noexcept;
private:
	std::string mAnnunciatorName;
	std::string mAuthorName;

	ma_sound_group mGroup;

	std::vector<ma_sound*> mBaseSounds; //base sounds have well-defined (i.e. it is explicitly written in the spec) order
	std::vector<AnnunciatorStationSound> mStationSounds;
	std::vector<AnnunciatorLineSound> mLineSounds; //line numbers and names
	bool mInitialized;

	ma_sound* getSoundFromBase(AnnunciatorBaseSound aSound) noexcept;
	AnnunciatorStationSound* getDataFromStationCode(const std::string_view aStationCode) noexcept;
	AnnunciatorStationSound* getDataFromStationCode(const uint32_t aStationCode) noexcept;
	AnnunciatorLineSound* getDataFromLine(const uint8_t aLine) noexcept;

	void playSound(ma_sound* aSound) noexcept;
	void playSoundCollection(std::vector<ma_sound*>& arSounds, const bool aWaitUntilDone = false) noexcept;
};

#endif
