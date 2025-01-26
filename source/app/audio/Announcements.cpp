#include "Announcements.hpp"

//TODO rewrite errors as exceptions?

#define AUDIO_FILE_EXTENSION ".wav"

Annunciator::Annunciator() noexcept : mInitialized(false) {}
Annunciator::Annunciator(const std::string_view aFilename) noexcept
	: mInitialized(false) {
	this->open(aFilename);
}

void Annunciator::open(const std::string_view aFilename) noexcept {
	this->destroy();

	ma_sound_group_init(getAudioEngine(), 0, NULL, &this->mGroup);

	std::ifstream fileHandle;
	fileHandle.open(aFilename.data(), std::ios::binary | std::ios::in);
	if(!fileHandle.is_open()) {
		std::cerr << LogLevel::ERROR << "Failed to open annuciator file " << aFilename << ".\n" << LogLevel::RESET;
		return;
	}

	std::string buffer;
	buffer.reserve(1024);

	//read header

	//ETAN

	readBytesToString(fileHandle, buffer, 4);
	if(buffer != "ETAN") {
		std::cerr << LogLevel::ERROR << "File header wrong! Should be ETAN, instead got " << buffer << '\n' << LogLevel::RESET;
		return;
	}

	//V
	readBytesToString(fileHandle, buffer, 2);
	if((*(uint16_t*)buffer.data()) != ANNUNCIATOR_FILE_FORMAT_VERSION) {
		std::cerr << LogLevel::ERROR << "File format version incompatible! Simulator version: " << ANNUNCIATOR_FILE_FORMAT_VERSION << ", file version: " << (*(uint16_t*)buffer.data()) << '\n' << LogLevel::RESET;
		return;
	}

	//D
	readBytesToString(fileHandle, buffer, 8);
	std::cout << "Annunciator file edit unix time: " << *((uint64_t*)buffer.data()) << '\n';

	//J
	std::getline(fileHandle, this->mAnnunciatorName, '\0');
	std::cout << "Annunciator name: " << this->mAnnunciatorName << '\n';

	//A
	std::getline(fileHandle, this->mAuthorName, '\0');
	std::cout << "Annunciator name: " << this->mAuthorName << '\n';

	//P
	std::string linePrefix;
	std::getline(fileHandle, linePrefix, '\0');
	std::cout << "Line audio file directory: " << linePrefix << '\n';

	//K
	readBytesToString(fileHandle, buffer, 1);
	uint8_t lineAmount = *((uint8_t*)buffer.data());
	readBytesToString(fileHandle, buffer, lineAmount + 1); //save a few reads by reading all lines and M
	std::cout << "Number line amount: " << (uint16_t)lineAmount << '\n';

	//L...L - lines
	this->mLineSounds.reserve(lineAmount);
	for(uint64_t j = 0; j < lineAmount; j++) {
		this->mLineSounds.push_back({});
		std::string filename = linePrefix;
		filename.append(std::to_string((uint16_t)*((uint8_t*)(&buffer[j])))).append(AUDIO_FILE_EXTENSION);
		this->mLineSounds.back().sound = new ma_sound;
		ma_result result = ma_sound_init_from_file(getAudioEngine(), filename.c_str(), 0, &this->mGroup, NULL, this->mLineSounds.back().sound);
		if(result != MA_SUCCESS) {
			std::cerr << LogLevel::ERROR << "Sound " << filename << " failed to load: " << ma_result_description(result) << "\n" << LogLevel::RESET;
			return;
		}
		this->mLineSounds.back().lineNumber = *((uint8_t*)(&buffer[j]));
		buffer.clear(); //implementations do not decrease capacity, this is guaranteed for vector but not this (realistically everyone still does this)
	}

	//M
	uint8_t specialLineAmount = *((uint8_t*)&buffer[lineAmount]);
	std::cout << "Non-numbered line amount: " << (uint16_t)specialLineAmount << '\n';

	//N...N - special lines
	this->mLineSounds.reserve(lineAmount + specialLineAmount);
	buffer.clear();
	for(uint64_t j = 0; j < specialLineAmount; j++) {
		std::getline(fileHandle, buffer, '\0');
		this->mLineSounds.push_back({});
		std::string filename = linePrefix;
		filename.append(buffer).append(AUDIO_FILE_EXTENSION);
		this->mLineSounds.back().sound = new ma_sound;
		ma_result result = ma_sound_init_from_file(getAudioEngine(), filename.c_str(), 0, &this->mGroup, NULL, this->mLineSounds.back().sound);
		if(result != MA_SUCCESS) {
			std::cerr << LogLevel::ERROR << "Sound " << filename << " failed to load: " << ma_result_description(result) << "\n" << LogLevel::RESET;
			return;
		}
		this->mLineSounds.back().lineName = buffer;
		buffer.clear(); //implementations do not decrease capacity, this is guaranteed for vector but not this (realistically everyone still does this)
	}

	//Z
	readBytesToString(fileHandle, buffer, 4);
	uint64_t amountStations = *((uint32_t*)buffer.data());
	std::cout << "Amount of stations: " << amountStations << '\n';

	//read base sounds
	this->mBaseSounds.reserve((uint64_t)AnnunciatorBaseSound::AMOUNT_SOUNDS);
	buffer.clear();
	for(uint64_t i = 0; i < (uint64_t)AnnunciatorBaseSound::AMOUNT_SOUNDS; i++) {
		std::getline(fileHandle, buffer, '\0');
		this->mBaseSounds.emplace_back();
		this->mBaseSounds.back() = new ma_sound;
		ma_result result = ma_sound_init_from_file(getAudioEngine(), buffer.append(AUDIO_FILE_EXTENSION).c_str(), 0, &this->mGroup, NULL, this->mBaseSounds.back());
		if(result != MA_SUCCESS) {
			std::cerr << LogLevel::ERROR << "Sound " << buffer << " failed to load: " << ma_result_description(result) << "\n" << LogLevel::RESET;
			return;
		}
		buffer.clear();
	}

	//read station list and load files
	this->mStationSounds.reserve(amountStations);
	for(uint64_t i = 0; i < amountStations; i++) {
		this->mStationSounds.emplace_back();

		//S
		readBytesToString(fileHandle, buffer, 4);
		this->mStationSounds.back().stationCode = *((uint32_t*)buffer.data());
		//N
		buffer.clear();
		std::getline(fileHandle, this->mStationSounds.back().stationName, '\0');

		//A and sound loading
		buffer.clear();
		std::getline(fileHandle, buffer, '\0');

		this->mStationSounds.back().sound = new ma_sound;

		ma_result result = ma_sound_init_from_file(getAudioEngine(), buffer.append(AUDIO_FILE_EXTENSION).c_str(), 0, &this->mGroup, NULL, this->mStationSounds.back().sound);

		if(result != MA_SUCCESS) {
			std::cerr << LogLevel::ERROR << "Sound " << buffer << " failed to load: " << ma_result_description(result) << "\n" << LogLevel::RESET;
			return;
		}

		//K and lines
		readBytesToString(fileHandle, buffer, 1);
		uint8_t lineAmount = *((uint8_t*)buffer.data());
		readBytesToString(fileHandle, buffer, lineAmount + 1); //save a few reads - all lines and M

		this->mStationSounds.back().lines.reserve(lineAmount);
		for(uint64_t j = 0; j < lineAmount; j++) this->mStationSounds.back().lines.push_back(*((uint8_t*)(&buffer[j])));

		//M and special lines
		uint8_t specialLineAmount = *((uint8_t*)(&buffer[lineAmount]));

		for(uint64_t j = 0; j < specialLineAmount; j++) {
			std::getline(fileHandle, buffer, '\0');
			this->mStationSounds.back().letteredLines.push_back(buffer);
			buffer.clear();
		}

		//flags
		readBytesToString(fileHandle, buffer, 1);
		this->mStationSounds.back().flags = *((uint8_t*)buffer.data());
	}

	fileHandle.close();
	this->mInitialized = true;
}

Annunciator& Annunciator::addBaseAnnouncement(const AnnunciatorBaseSound aSound) noexcept {
	this->mQueue.push_back(this->getSoundFromBase(aSound));
	return *this;
}
Annunciator& Annunciator::addAnnouncementCurrent(const std::string_view aStationCode) noexcept {
	//jingle + station name + this stop on request + transfer to metro, rail + closure announcement - IN THIS ORDER

	this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::JINGLE));

	AnnunciatorStationSound* soundData = getDataFromStationCode(aStationCode);
	if(soundData == nullptr) {
		std::cerr << LogLevel::ERROR << "No sound found for station code " << aStationCode << "\n" << LogLevel::RESET;
		return *this;
	}
	this->mQueue.push_back(soundData->sound);

	if((soundData->flags & STATION_FLAG_ON_REQUEST) != 0) {
		this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::THIS_STOP_ON_REQUEST));
	}
	if((soundData->flags & STATION_FLAG_METRO_TRANSFER) != 0) {
		this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::TRANSFER_TO_METRO));
	}
	if((soundData->flags & STATION_FLAG_TRAIN_TRANSFER) != 0) {
		this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::TRANSFER_TO_RAIL));
	}
	if((soundData->flags & STATION_FLAG_FUNICULAR_CLOSURE) != 0) {
		this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::FUNICULAR_CLOSED));
	}

	return *this;
}
Annunciator& Annunciator::addAnnouncementNext(const std::string_view aNextStationCode) noexcept {
	//next stop + station name + sometimes on request, closure announcement
	//work in similar fashion to playAnnouncementCurrent

	AnnunciatorStationSound* soundData = getDataFromStationCode(aNextStationCode);
	if(soundData == nullptr) {
		std::cerr << LogLevel::ERROR << "No sound found for station code " << aNextStationCode << "\n" << LogLevel::RESET;
		return *this;
	}

	this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::NEXT_STOP));
	this->mQueue.push_back(soundData->sound);

	if((soundData->flags & STATION_FLAG_ON_REQUEST) != 0) {
		this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::NEXT_STOP_ON_REQUEST));
	}

	//same sound cannot have 2 starting points - we set after it played
	if((soundData->flags & STATION_FLAG_METRO_CLOSURE) != 0) {
		this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::WARNING_FOR_PASSENGERS));
		this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::THE_METRO_STATION_IS));
		this->mQueue.push_back(soundData->sound); //TODO later add separate name for metro
		this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::CURRENTLY_CLOSED));
	}

	if((soundData->flags & STATION_FLAG_FUNICULAR_CLOSURE) != 0) {
		this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::FUNICULAR_CLOSED));
	}

	return *this;
}

Annunciator& Annunciator::addAnnouncement(const std::string_view aStationCode,const std::string_view aNextStationCode) noexcept {
	this->addAnnouncementCurrent(aStationCode);
	this->addAnnouncementNext(aNextStationCode);
	return *this;
}

Annunciator& Annunciator::addAnnouncementLineChange(const std::string_view aStationCode, const uint8_t aNewLine, const std::string_view aNewEndStationCode) noexcept {
	//line change + from station + STATION + is routed as line + LINE + in the direction of + ENDSTOP

	this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::LINE_ROUTE_CHANGE));
	this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::FROM_THE_STATION));

	AnnunciatorStationSound* soundData = getDataFromStationCode(aStationCode);
	if(soundData == nullptr) {
		std::cerr << LogLevel::ERROR << "No sound found for station code " << aStationCode << "\n" << LogLevel::RESET;
		return *this;
	}
	this->mQueue.push_back(soundData->sound);
	this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::CONTINUES_JOURNEY_AS));

	AnnunciatorLineSound* lineSoundData = getDataFromLine(aNewLine);
	if(lineSoundData == nullptr) {
		std::cerr << LogLevel::ERROR << "No sound found for line " << aNewLine << "\n" << LogLevel::RESET;
		return *this;
	}
	this->mQueue.push_back(lineSoundData->sound);
	this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::DIRECTION));

	AnnunciatorStationSound* endStationData = getDataFromStationCode(aNewEndStationCode);
	if(endStationData == nullptr) {
		std::cerr << LogLevel::ERROR << "No sound found for end station code " << aNewEndStationCode << "\n" << LogLevel::RESET;
		return *this;
	}
	this->mQueue.push_back(endStationData->sound);

	return *this;
}
Annunciator& Annunciator::addAnnouncementTerminus() {
	this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::TERMINUS));
	this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::TERMINUS_OTHER_LANGUAGE));
	return *this;
}
Annunciator& Annunciator::addAnnouncementStart(const uint8_t aNewLine, const std::string_view aEndStationCode) {
	this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::LINE));
	AnnunciatorLineSound* lineSoundData = getDataFromLine(aNewLine);
	if(lineSoundData == nullptr) {
		std::cerr << LogLevel::ERROR << "No sound found for line " << aNewLine << "\n" << LogLevel::RESET;
		return *this;
	}
	this->mQueue.push_back(lineSoundData->sound);
	this->mQueue.push_back(getSoundFromBase(AnnunciatorBaseSound::DIRECTION));
	AnnunciatorStationSound* soundData = getDataFromStationCode(aEndStationCode);
	if(soundData == nullptr) {
		std::cerr << LogLevel::ERROR << "No sound found for station code " << aEndStationCode << "\n" << LogLevel::RESET;
		return *this;
	}
	this->mQueue.push_back(soundData->sound);
	return *this;
}

void Annunciator::play(bool aWaitUntilDone) {
	this->playSoundCollection(this->mQueue, aWaitUntilDone);
}
void Annunciator::clearQueue() {
	this->mQueue.clear();
}

void Annunciator::setVolume(const float aVolume) noexcept {
	ma_sound_group_set_volume(&this->mGroup, aVolume);
}

void Annunciator::destroy() noexcept {
	if(!this->mInitialized) return;

	for(ma_sound*& sound : this->mBaseSounds) {
		ma_sound_uninit(sound);
		delete sound;
		sound = nullptr;
	}
	for(AnnunciatorStationSound& sound : this->mStationSounds) {
		ma_sound_uninit(sound.sound);
		delete sound.sound;
		sound.sound = nullptr;
	}
	for(AnnunciatorLineSound& sound : this->mLineSounds) {
		ma_sound_uninit(sound.sound);
		delete sound.sound;
		sound.sound = nullptr;
	}
	ma_sound_group_uninit(&this->mGroup);

	this->mAnnunciatorName.clear();
	this->mAuthorName.clear();
	this->mBaseSounds.clear();
	this->mStationSounds.clear();
	this->mLineSounds.clear();

	this->mInitialized = false;
}

std::string_view Annunciator::getStationName(const std::string_view aStationCode) const noexcept {
	return this->getStationName(*((uint32_t*)aStationCode.data()));
}
std::string_view Annunciator::getStationName(const uint32_t aStationCode) const noexcept {
	for(const AnnunciatorStationSound& sound : this->mStationSounds) {
		if(sound.stationCode == aStationCode) return sound.stationName;
	}
	return "";
}

Annunciator::~Annunciator() noexcept {
	this->destroy();
}

ma_sound* Annunciator::getSoundFromBase(AnnunciatorBaseSound aSound) noexcept {
	return this->mBaseSounds[(uint64_t)aSound];
}
AnnunciatorStationSound* Annunciator::getDataFromStationCode(const std::string_view aStationCode) noexcept {
	return this->getDataFromStationCode(*((uint32_t*)aStationCode.data()));
}
AnnunciatorStationSound* Annunciator::getDataFromStationCode(const uint32_t aStationCode) noexcept {
	for(AnnunciatorStationSound& sound : this->mStationSounds) {
		if(sound.stationCode == aStationCode) return &sound;
	}
	return nullptr;
}
AnnunciatorLineSound* Annunciator::getDataFromLine(const uint8_t aLine) noexcept {
	for(AnnunciatorLineSound& sound : this->mLineSounds) {
		if(sound.lineNumber == aLine) return &sound;
	}
	return nullptr;
}

void Annunciator::playSound(ma_sound* aSound) noexcept {
	if(aSound == nullptr) return; //ignore if nullptr
	ma_sound_seek_to_pcm_frame(aSound, 0);
	ma_sound_start(aSound);
}

struct LatePlaybackInfo {
	std::vector<uint64_t> offsets;
	uint64_t currentRepeat = 0;
};

static void soundPlayEndCallback(void* aDataStruct, ma_sound* aSound) noexcept {
	LatePlaybackInfo* structPtr = (LatePlaybackInfo*)aDataStruct;

	ma_sound_set_start_time_in_pcm_frames(aSound, structPtr->offsets[structPtr->currentRepeat]);
	ma_sound_seek_to_pcm_frame(aSound, 0);
	ma_sound_start(aSound);
	structPtr->currentRepeat++;

	//if last playback
	if(structPtr->offsets.size() == structPtr->currentRepeat) {
		ma_sound_set_end_callback(aSound, NULL, NULL);
		delete structPtr;
	}
	else {
		ma_sound_set_end_callback(aSound, soundPlayEndCallback, structPtr);
	}
}

void Annunciator::playSoundCollection(std::vector<ma_sound*>& aSounds, const bool aWaitUntilDone) noexcept {
	//aAmount should be non-zero

	//for each possible sound
	// - check if played, and if yes:
	//  - get its length
	// (saving at beginning would require another struct + when we dont use it we set to zero which massively simplifies calculations later on)
	//  - set that it is last

	//have to allocate on heap - repeat may play after end of function
	//end of functions unallocates stack
	//have to allocate here since we take pointer
	std::vector<LatePlaybackInfo*> soundInfo;
	soundInfo.resize(aSounds.size());

	ma_sound* lastSound = aSounds[0];
	ma_uint64 currentPCMTime = ma_engine_get_time_in_pcm_frames(getAudioEngine());

	uint64_t timeOffset = 0;
	for(uint64_t i = 0; i < aSounds.size(); i++) {
		ma_uint64 PCMLength = 0;
		ma_sound_get_length_in_pcm_frames(aSounds[i], &PCMLength);
		lastSound = aSounds[i];

		bool isRepeating = false;
		//first time play as normal, then change
		//find if sound was played before
		for(uint64_t j = 0; j < i; j++) {
			if(aSounds[i] == aSounds[j]) {
				//sound is repeated - j is id of first repeat, i of current

				if(soundInfo[j] == nullptr) {
					soundInfo[j] = new LatePlaybackInfo;
					ma_sound_set_end_callback(aSounds[j], soundPlayEndCallback, soundInfo[j]);
				}

				soundInfo[j]->offsets.push_back(currentPCMTime + timeOffset);

				isRepeating = true;
				break;
			}
		}

		if(!isRepeating) {
			//do not override repeating sounds
			ma_sound_set_start_time_in_pcm_frames(aSounds[i], currentPCMTime + timeOffset);
			this->playSound(aSounds[i]);
		}

		timeOffset += PCMLength;
	}

	this->mQueue.clear();

	if(aWaitUntilDone) {
		while(!ma_sound_at_end(lastSound)) std::this_thread::sleep_for(5us);
	}
}
