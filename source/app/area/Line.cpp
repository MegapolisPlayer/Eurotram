#include "Line.hpp"

static constexpr std::string_view scLineUnitializedString = "Line object uninitialized!";

Line::Line() noexcept
: mCurrentLoopId(0), mCurrentStationId(-1), mCurrentSwitchId(0), mDelay(0), mWeather(0), mStartDate(0), mInitialized(false), mAnnunciator(nullptr) {}

Line::Line(const std::string_view aFilename, Annunciator* aAnnunciator) noexcept
: mCurrentLoopId(0), mCurrentStationId(-1), mCurrentSwitchId(0), mDelay(0), mWeather(0), mStartDate(0), mInitialized(false), mAnnunciator(nullptr) {
	this->open(aFilename, aAnnunciator);
}

Line::Line(Line&& aOther) noexcept
	: mLoops(std::move(aOther.mLoops)), mCurrentLoopId(aOther.mCurrentLoopId), mCurrentStationId(aOther.mCurrentStationId), mCurrentSwitchId(aOther.mCurrentSwitchId),
	mDelay(aOther.mDelay), mWeather(aOther.mWeather), mStartDate(aOther.mStartDate), mInitialized(aOther.mInitialized),
	mAnnunciator(aOther.mAnnunciator), mLineName(std::move(aOther.mLineName)), mAuthorName(std::move(aOther.mAuthorName)) {
	aOther.mAnnunciator = nullptr;
}
Line& Line::operator=(Line&& aOther) noexcept {
	this->mLoops = std::move(aOther.mLoops);
	this->mCurrentLoopId = aOther.mCurrentLoopId;
	this->mCurrentStationId = aOther.mCurrentStationId;
	this->mCurrentSwitchId = aOther.mCurrentSwitchId;
	this->mDelay = aOther.mDelay;
	this->mWeather = aOther.mWeather;
	this->mStartDate = aOther.mStartDate;
	this->mInitialized = std::move(aOther.mInitialized);
	this->mAnnunciator = std::move(aOther.mAnnunciator);
	aOther.mAnnunciator = nullptr;
	this->mLineName = std::move(aOther.mLineName);
	this->mAuthorName = std::move(aOther.mAuthorName);
	return *this;
}

void Line::open(const std::string_view aFilename, Annunciator* aAnnunciator) noexcept {
	this->destroy();

	this->mAnnunciator = aAnnunciator;

	std::ifstream fileHandle;
	fileHandle.open(aFilename.data(), std::ios::binary | std::ios::in);
	if(!fileHandle.is_open()) {
		std::cerr << LogLevel::ERROR << "Failed to open line file " << aFilename << ".\n" << LogLevel::RESET;
		return;
	}

	std::string buffer;
	buffer.reserve(1024);

	//read header

	//TODO maybe abstract error checks into separate functions to duplicate
	//TODO - load whole file into memory, then process (not yet problem, files arent so large)

	//ETSC
	readBytesToString(fileHandle, buffer, 4);
	if(buffer != "ETSC") {
		std::cerr << LogLevel::ERROR << "File header wrong! Should be ETSC, instead got " << buffer << '\n' << LogLevel::RESET;
		return;
	}

	//V
	readBytesToString(fileHandle, buffer, 2);
	if((*(uint16_t*)buffer.data()) != LINE_FILE_FORMAT_VERSION) {
		std::cerr << LogLevel::ERROR << "File format version incompatible! Simulator version: " << LINE_FILE_FORMAT_VERSION << ", file version: " << (*(uint16_t*)buffer.data()) << '\n' << LogLevel::RESET;
		return;
	}

	//D
	readBytesToString(fileHandle, buffer, 8);
	std::cout << "Line file edit unix time: " << *((uint64_t*)buffer.data()) << '\n';

	//S
	readBytesToString(fileHandle, buffer, 8);
	this->mStartDate = *((uint64_t*)buffer.data());
	std::cout << "Line start date: " << this->mStartDate << '\n';

	//W
	readBytesToString(fileHandle, buffer, 2);
	this->mWeather = *((uint16_t*)buffer.data());

	//N
	readBytesToString(fileHandle, buffer, 1);
	uint8_t loopAmount = *((uint8_t*)buffer.data());

	//J
	std::getline(fileHandle, this->mLineName, '\0');
	std::cout << "Line name: " << this->mLineName << '\n';

	//A
	std::getline(fileHandle, this->mAuthorName, '\0');
	std::cout << "Author name: " << this->mAuthorName << '\n';

	//loops
	this->mLoops.reserve(loopAmount);
	uint64_t timePassed = 0; //station time is save as time from previous - we convert to absolute value
	for(uint8_t i = 0; i < loopAmount; i++) {
		this->mLoops.push_back({});

		//L
		readBytesToString(fileHandle, buffer, 1);
		this->mLoops.back().lineNumber = *(uint8_t*)buffer.data();

		//M
		readBytesToString(fileHandle, buffer, 1);
		this->mLoops.back().lineNumber2 = LineData::LineType(*(uint8_t*)buffer.data());

		//I
		readBytesToString(fileHandle, buffer, 2);
		this->mLoops.back().orderNumber = *(uint16_t*)buffer.data();

		//N
		readBytesToString(fileHandle, buffer, 2);
		uint16_t stationAmount = *(uint16_t*)buffer.data();

		this->mLoops.back().stations.reserve(stationAmount);
		std::cout << "Line loop station amount: " << stationAmount << '\n';
		for(uint16_t j = 0; j < stationAmount; j++) {
			this->mLoops.back().stations.push_back({});

			//S byte - check it
			readBytesToString(fileHandle, buffer, 1);
			if(buffer != "S") {
				std::cerr << LogLevel::ERROR << "Invalid file: station byte missing!\n" << LogLevel::RESET;
				return;
			}

			//I
			readBytesToString(fileHandle, buffer, 4);
			this->mLoops.back().stations.back().mapId = *(uint32_t*)buffer.data();

			//C
			readBytesToString(fileHandle, buffer, 4);
			this->mLoops.back().stations.back().stationCode = buffer;

			//T
			readBytesToString(fileHandle, buffer, 2);
			timePassed += *(uint16_t*)buffer.data();
			this->mLoops.back().stations.back().arrivalTime = timePassed;

			//M
			readBytesToString(fileHandle, buffer, 2);
			this->mLoops.back().stations.back().timeInStation = *(uint16_t*)buffer.data();

			//K
			readBytesToString(fileHandle, buffer, 1);
			this->mLoops.back().stations.back().isControlPoint = *(uint8_t*)buffer.data();

			//N
			readBytesToString(fileHandle, buffer, 1);
			uint8_t switchAmount = *(uint8_t*)buffer.data();

			this->mLoops.back().stations.back().switches.reserve(switchAmount);
			for(uint8_t k = 0; k < switchAmount; k++) {
				this->mLoops.back().stations.back().switches.push_back({});

				//W byte - check it
				readBytesToString(fileHandle, buffer, 1);
				if(buffer != "W") {
					std::cerr << LogLevel::ERROR << "Invalid file: switch byte missing!\n" << LogLevel::RESET;
					return;
				}

				//I
				readBytesToString(fileHandle, buffer, 4);
				this->mLoops.back().stations.back().switches.back().mapId = *(uint32_t*)buffer.data();

				//D
				readBytesToString(fileHandle, buffer, 1);
				this->mLoops.back().stations.back().switches.back().direction = LineData::SwitchDirection(*(uint8_t*)buffer.data());
			}
		}
	}

	fileHandle.close();

	std::cout << "Line loop amount: " << this->mLoops.size() << '\n';
	std::cout << "First line loop station amount: " << this->mLoops[0].stations.size() << '\n';

	this->mInitialized = true;
}

uint64_t Line::next(const uint64_t aMinuteTime) noexcept {
	if(!this->mInitialized) return UINT64_MAX;

	this->mCurrentStationId++;

	if(this->mCurrentStationId == 0) {
		//first station - play start announcement first
		uint8_t lineNumber2 = this->mLoops[this->mCurrentLoopId].lineNumber2;
		if(lineNumber2 == LineData::LINE_TYPE_NORMAL_NUMBERED) {
			this->mAnnunciator->addAnnouncementStart(
				this->mLoops[this->mCurrentLoopId].lineNumber, this->getLastStationCode()
			);
		}
	}

	this->mDelay = aMinuteTime
	- this->mLoops[this->mCurrentLoopId].stations[this->mCurrentStationId].arrivalTime
	- this->mLoops[this->mCurrentLoopId].stations[this->mCurrentStationId].timeInStation;

	this->mAnnunciator->addAnnouncementCurrent(this->getCurrentStationCode());

	if(this->isStationLast()) {
		this->mAnnunciator->addAnnouncementTerminus().play();
		this->nextLoop();
		return 0;
	}
	else {
		this->mAnnunciator->addAnnouncementNext(this->getNextStationCode()).play();
		return this->mLoops[this->mCurrentLoopId].stations[this->mCurrentStationId].timeInStation;
	}
}

bool Line::nextLoop() noexcept {
	if(this->isLoopLast()) {
		//end scenario
		this->reset();
		return false;
	}
	else {
		this->mCurrentLoopId++;
		this->mCurrentStationId = -1;
		this->mDelay = 0;
		return true;
	}
}

std::optional<LineData::Switch> Line::getNextSwitch() noexcept {
	if(this->isStationLast() || this->isLoopLast()) {
		return {}; //empty optional
	}

	auto& currentLoop = this->mLoops[this->mCurrentLoopId];
	auto& currentStation = currentLoop.stations[this->mCurrentStationId];

	if(this->mCurrentSwitchId == (int64_t)currentStation.switches.size()-1) {
		this->mCurrentSwitchId = 0;
		return {};
	}

	this->mCurrentSwitchId++;
	return currentStation.switches[this->mCurrentSwitchId-1];
}

std::vector<LineData::Switch>& Line::getSwitchesToNextStop() noexcept {
	return this->mLoops[this->mCurrentLoopId].stations[this->mCurrentStationId].switches;
}

void Line::reset() noexcept {
	//dont destroy - just reset to beginning
	this->mCurrentLoopId = 0;
	this->mCurrentStationId = -1;
	this->mDelay = 0;
}
void Line::destroy() noexcept {
	if(!this->mInitialized) return;

	this->mLoops.clear();
	this->mCurrentLoopId = 0;
	this->mCurrentStationId = -1;
	this->mDelay = 0;
	this->mInitialized = false;
	this->mAnnunciator = nullptr;
}

uint64_t Line::getTimeInCurrentStation() const noexcept {
	if(!this->mInitialized) return UINT64_MAX;
	if(this->mCurrentStationId == -1) return this->mLoops[this->mCurrentLoopId].stations[0].timeInStation;
	return this->mLoops[this->mCurrentLoopId].stations[this->mCurrentStationId].timeInStation;
}

const std::string_view Line::getFirstStationCode() const noexcept {
	if(!this->mInitialized) return scLineUnitializedString;
	return this->mLoops[this->mCurrentLoopId].stations.front().stationCode;
}
const std::string_view Line::getFirstStationName() const noexcept {
	if(!this->mInitialized) return scLineUnitializedString;
	return this->mAnnunciator->getStationName(
		this->mLoops[this->mCurrentLoopId].stations.front().stationCode
	);
}

const std::string_view Line::getCurrentStationCode() const noexcept {
	if(!this->mInitialized) return scLineUnitializedString;
	if(this->mCurrentStationId == -1) return this->mLoops[this->mCurrentLoopId].stations[0].stationCode;
	return this->mLoops[this->mCurrentLoopId].stations[this->mCurrentStationId].stationCode;
}
const std::string_view Line::getCurrentStationName() const noexcept {
	if(!this->mInitialized) return scLineUnitializedString;
	std::string_view code;
	if(this->mCurrentStationId == -1) code = this->mLoops[this->mCurrentLoopId].stations[0].stationCode;
	else code = this->mLoops[this->mCurrentLoopId].stations[this->mCurrentStationId].stationCode;
	return this->mAnnunciator->getStationName(code);
}

const std::string_view Line::getNextStationCode() const noexcept {
	if(!this->mInitialized) return scLineUnitializedString;
	if(this->isStationLast()) return "";
	return this->mLoops[this->mCurrentLoopId].stations[this->mCurrentStationId+1].stationCode;
}
const std::string_view Line::getNextStationName() const noexcept {
	if(!this->mInitialized) return scLineUnitializedString;
	if(this->isStationLast()) return "";
	return this->mAnnunciator->getStationName(
		this->mLoops[this->mCurrentLoopId].stations[this->mCurrentStationId+1].stationCode
	);
}

const std::string_view Line::getLastStationCode() const noexcept {
	if(!this->mInitialized) return scLineUnitializedString;
	return this->mLoops[this->mCurrentLoopId].stations.back().stationCode;
}
const std::string_view Line::getLastStationName() const noexcept {
	if(!this->mInitialized) return scLineUnitializedString;
	return this->mAnnunciator->getStationName(
		this->mLoops[this->mCurrentLoopId].stations.back().stationCode
	);
}

uint64_t Line::getNextStationTime() const noexcept {
	if(!this->mInitialized) return UINT64_MAX;
	if(this->isStationLast()) return this->mLoops[this->mCurrentLoopId].stations.back().arrivalTime;
	return this->mLoops[this->mCurrentLoopId].stations[this->mCurrentStationId + 1].arrivalTime;
}
bool Line::isNextStationControlPoint() const noexcept {
	if(this->isStationLast() || !this->mInitialized) return false;
	return this->mLoops[this->mCurrentLoopId].stations[this->mCurrentStationId + 1].isControlPoint;
}
uint64_t Line::getDelay() const noexcept {
	return this->mDelay;
}

const std::string Line::getLineNumber() const noexcept {
	if(!this->mInitialized) return scLineUnitializedString.data();
	//we assume everything is in UTF8 (we also set it at program init)
	uint16_t lineNumber = this->mLoops[this->mCurrentLoopId].lineNumber;
	uint16_t lineNumber2 = this->mLoops[this->mCurrentLoopId].lineNumber2;
	switch(lineNumber2) {
		case(LineData::LINE_TYPE_NORMAL_NUMBERED):
			return std::string(std::to_string(lineNumber));
		case(LineData::LINE_TYPE_HANDLING):
			return "Manipulační jízda";
		case(LineData::LINE_TYPE_SERVICE):
			return "Služební jízda";
		case(LineData::LINE_TYPE_SPECIAL):
			return "Zvláštní jízda";
		case(LineData::LINE_TYPE_PRACTICE):
			return "Cvičná jízda";
		case(LineData::LINE_TYPE_TEST):
			return "Zkušební jízda";
		default:
			std::string temp;
			temp.push_back(lineNumber);
			temp.push_back(lineNumber2);
			return temp;
	}
}
uint64_t Line::getOrderNumber() const noexcept {
	if(!this->mInitialized) return UINT64_MAX;
	return this->mLoops[this->mCurrentLoopId].orderNumber;
}
uint64_t Line::getLineType() const noexcept {
	if(!this->mInitialized) return UINT64_MAX;
	return this->mLoops[this->mCurrentLoopId].lineNumber2;
}

uint64_t Line::getAmountOfStationsInCurrentLoop() const noexcept {
	if(!this->mInitialized) return UINT64_MAX;
	return this->mLoops[this->mCurrentLoopId].stations.size();
}
uint64_t Line::getAmountOfStationsInNextLoop() const noexcept {
	if(this->isLoopLast()) return 0;
	return this->mLoops[this->mCurrentLoopId+1].stations.size();
}

uint16_t Line::getWeather() const noexcept {
	return this->mWeather;
}
uint64_t Line::getStartDate() const noexcept {
	return this->mStartDate;
}

bool Line::isStationLast() const noexcept {
	if(!this->mInitialized) return true;
	return this->mCurrentStationId >= (int64_t)this->mLoops[this->mCurrentLoopId].stations.size() - 1;
}
bool Line::isLoopLast() const noexcept {
	return this->mCurrentLoopId >= this->mLoops.size() - 1;
}

LineData::Station Line::getNextStation() const noexcept {

}

Line::~Line() noexcept {
	this->destroy();
}

namespace UI {
	void drawLineInfoWindow(Line& aLine) noexcept {
		ImGui::Begin("Line");

		ImGui::Text("Line: %s (order %zu)", aLine.getLineNumber().c_str(), aLine.getOrderNumber());
		ImGui::Text("Starting stop: %s (%s)", aLine.getFirstStationName().data(), aLine.getFirstStationCode().data());
		ImGui::Text("Final stop: %s (%s)", aLine.getLastStationName().data(), aLine.getLastStationCode().data());
		ImGui::Text("Next stop: %s (%s)", aLine.getNextStationName().data(), aLine.getNextStationCode().data());

		std::string controlPoints;
		controlPoints.reserve(1000);

		ImGui::Text("Control points: %s", "IPPV, FLOR");

		ImGui::End();
	}
}
