#ifndef EUROTRAM_AREA_LINE
#define EUROTRAM_AREA_LINE
#include "../audio/Audio.hpp"

constexpr uint64_t LINE_FILE_FORMAT_VERSION = 0;

namespace LineData {
	enum struct SwitchDirection : int8_t {
		STRAIGHT = 0,
		LEFT = 1,
		RIGHT = 2,
		NO_SET = 3, //against the switch, no need to set since we go to before segment
	};

	struct Switch {
		uint32_t mapId;
		SwitchDirection direction;
	};

	struct Station {
		uint32_t mapId;
		std::string stationCode;
		uint32_t arrivalTime; //in minutes since start of scenario
		uint32_t timeInStation;
		bool isControlPoint;
		std::vector<Switch> switches; //AFTER this station
	};

	enum LineType : uint8_t {
		LINE_TYPE_NORMAL_NUMBERED = 0,
		LINE_TYPE_HANDLING = 1,
		LINE_TYPE_SERVICE = 2,
		LINE_TYPE_SPECIAL = 3,
		LINE_TYPE_PRACTICE = 4,
		LINE_TYPE_TEST = 5,
		LINE_TYPE_LETTERED = 6, //this value and above mean line is lettered (e.g. XC)
	};

	struct Loop {
		uint8_t lineNumber;
		LineType lineNumber2; //if this value is <= 5, make according to LineType enum
		uint16_t orderNumber;
		std::vector<Station> stations;
	};

	enum LineWeather : uint16_t {
		WEATHER_WIND           = 0b0000000000000001,
		WEATHER_RAIN           = 0b0000000000000010,
		WEATHER_LIGHTING       = 0b0000000000000100,
		WEATHER_FOG            = 0b0000000000001000,
		WEATHER_SNOW           = 0b0000000000010000,
		WEATHER_SEASONS_SPRING = 0b0001000000000000,
		WEATHER_SEASONS_SUMMER = 0b0010000000000000,
		WEATHER_SEASONS_AUTUMN = 0b0100000000000000,
		WEATHER_SEASONS_WINTER = 0b1000000000000000,
	};
};

class Line;

namespace UI {
	void drawLineInfoWindow(Line& aLine) noexcept;
}

class Line {
	friend void UI::drawLineInfoWindow(Line& aLine) noexcept;
	public:
	//just initializes class - call open() later
	Line() noexcept;
	//loads a .etscr file - extension required!
	Line(const std::string_view aFilename, Annunciator* aAnnunciator) noexcept;

	Line(Line&& aOther) noexcept;
	Line(Line& aOther) noexcept = delete;
	Line& operator=(Line&& aOther) noexcept;
	Line& operator=(Line& aOther) noexcept = delete;

	void open(const std::string_view aFilename, Annunciator* aAnnunciator) noexcept;

	//plays next announcement, returns how long to stay at station currently announced
	//also updates delay - pass time since start of scenarion in minutes
	uint64_t next(const uint64_t aMinuteTime) noexcept;

	//returns false if scenario at end
	bool nextLoop() noexcept;

	//returns false if station last
	bool nextStation() noexcept;

	//call when passed switch - gets new one at each call
	std::optional<LineData::Switch> getNextSwitch() noexcept;

	//for current stop
	//detection of wrong direction at switch - track if stations not announced correctly - if yes and reach switch GAME OVER TODO
	std::vector<LineData::Switch>& getSwitchesToNextStop() noexcept;

	void playCurrentAnnouncement() noexcept;

	void reset() noexcept; //resets line to original state
	void destroy() noexcept; //destroys object, have to remake with open()

	uint64_t getTimeInCurrentStation() const noexcept;

	const std::string_view getFirstStationCode() const noexcept;
	const std::string_view getFirstStationName() const noexcept;
	const std::string_view getCurrentStationCode() const noexcept;
	const std::string_view getCurrentStationName() const noexcept;
	const std::string_view getNextStationCode() const noexcept;
	const std::string_view getNextStationName() const noexcept;
	const std::string_view getLastStationCode() const noexcept;
	const std::string_view getLastStationName() const noexcept;

	uint64_t getNextStationTime() const noexcept;
	bool isNextStationControlPoint() const noexcept;
	uint64_t getDelay() const noexcept;

	//always returns a string_view - if line numerical, returns it in string_view form
	//returns line names if special line
	const std::string getLineNumber() const noexcept;
	uint64_t getOrderNumber() const noexcept;
	uint64_t getLineType() const noexcept;

	uint64_t getAmountOfStationsInCurrentLoop() const noexcept;
	uint64_t getAmountOfStationsInNextLoop() const noexcept;

	uint16_t getWeather() const noexcept;
	uint64_t getStartDate() const noexcept;

	bool isStationLast() const noexcept;
	bool isLoopLast() const noexcept;

	LineData::Station getNextStation() const noexcept;

	uint64_t getFirstLoopTrack() const noexcept;
	//first - type (letter S/N in utf8), second - id in said list
	std::pair<uint8_t, uint64_t> getFirstNodePassed() const noexcept;

	~Line() noexcept;
private:
	std::vector<LineData::Loop> mLoops;
	uint64_t mCurrentLoopId;
	int64_t mCurrentStationId; //this value can be -1 before first station announcement
	int64_t mCurrentSwitchId; //this value can be -1 if no more switches before stop TODO

	uint64_t mFirstPointId;
	uint8_t mFirstPointType; //ASCII letters S/N for switch/node

	uint64_t mDelay;
	uint16_t mWeather;
	uint64_t mStartDate;
	bool mInitialized;

	Annunciator* mAnnunciator;
	std::string mLineName;
	std::string mAuthorName;
};

namespace UI {
	void drawLineInfoWindow(Line& aLine) noexcept;
}

#endif
