#ifndef EUROTRAM_FREEUTILS
#define EUROTRAM_FREEUTILS
#include "Setup.hpp"

#define DISCARD(x) (void)x

//IO UTILITIES

namespace ConsoleColor {
	#ifdef _WIN32
	enum ConsoleStreamColor : uint8_t {
		BLACK = 0,
		BLUE,
		GREEN,
		CYAN,
		RED,
		PURPLE,
		BROWN,
		LIGHT_GREY,
		GREY,
		LIGHT_BLUE,
		LIGHT_GREEN,
		LIGHT_CYAN,
		LIGHT_RED,
		PINK,
		YELLOW,
		WHITE,
		DEFAULT = UINT8_MAX
	};
	#endif
	#ifdef __linux__
	enum ConsoleStreamColor : uint8_t {
		BLACK = 30,
		RED = 31,
		GREEN = 32,
		BROWN,
		BLUE = 34,
		PURPLE = 35,
		CYAN = 36,
		LIGHT_GREY = 37,
		GREY = 90,
		LIGHT_RED = 91,
		LIGHT_GREEN = 92,
		YELLOW = 93,
		LIGHT_BLUE = 94,
		PINK = 95,
		LIGHT_CYAN = 96,
		WHITE = 97,
		DEFAULT = 39
	};
	#endif
}

void setOutputColor(
	const ConsoleColor::ConsoleStreamColor aForeground,
	const ConsoleColor::ConsoleStreamColor aBackground = ConsoleColor::DEFAULT
) noexcept;

void setUTF8Encoding() noexcept;

enum struct LogLevel : uint8_t {
	OK = 0,
	ERROR,
	WARNING,
	SUCCESS,
	RESET
};

std::ostream& operator<<(std::ostream& aStream, const LogLevel aLevel) noexcept;
std::string readFile(std::fstream& aStream, const std::string_view aFilepath) noexcept;

typedef std::chrono::system_clock::time_point TimerType;

// TIME UTILITIES

class TimerAverage;

class Timer {
public:
	Timer() noexcept;

	void start() noexcept;
	void end() noexcept;
	void end(TimerAverage& aAvg) noexcept; //saves time to TimerAverage

	uint64_t getMS() const noexcept;
	uint64_t getUS() const noexcept;
	uint64_t getNS() const noexcept;

	float getMSfloat() const noexcept;
	float getUSfloat() const noexcept;

	~Timer() noexcept;
private:
	std::chrono::system_clock::time_point mStart;
	std::chrono::system_clock::duration mDuration;
};

class TimerAverage {
	friend class Timer;
public:
	TimerAverage() noexcept;

	void addTimer(const Timer& aTimer) noexcept;

	uint64_t getAverageMS() const noexcept;
	float getAverageMSfloat() const noexcept;

	uint64_t getAverageUS() const noexcept;
	float getAverageUSfloat() const noexcept;

	~TimerAverage() noexcept;
private:
	uint64_t mMicroseconds;
	uint64_t mAmount;
};

// OTHER

void readBytesToString(std::ifstream& aStream, std::string& aBuffer, const uint64_t aBytes) noexcept;

template<typename tType>
concept ContiguousStandardNotationContainer = requires(tType a) {
	{ a.data() } -> std::same_as<typename std::remove_reference<tType>::type::value_type *>;
	{ a.size() } -> std::same_as<size_t>;
};

template<typename tType>
concept MultipliableByFloat = requires(tType a, float b) {
	a * b;
};

template<typename tType>
concept Multipliable = requires(tType a, tType b) {
	a * b;
};

template<MultipliableByFloat tType, std::size_t tAmount>
std::array<tType, tAmount> operator*(std::array<tType, tAmount> aArray, const float aMultiplier) noexcept {
	std::transform(aArray.begin(), aArray.end(), aArray.begin(), [&](tType a) {
		return a * aMultiplier;
	});
	return aArray;
}

template<Multipliable tType, std::size_t tAmount>
std::array<tType, tAmount> operator*(std::array<tType, tAmount> aArray1, std::array<tType, tAmount> aArray2) noexcept {
	return {aArray1[0] * aArray2[0], aArray1[1] * aArray2[1], aArray1[2]*aArray2[2], aArray1[3]*aArray1[3]};
}

std::ostream& operator<<(std::ostream& aStream, const glm::vec2& aVector) noexcept;
std::ostream& operator<<(std::ostream& aStream, const glm::vec3& aVector) noexcept;
std::ostream& operator<<(std::ostream& aStream, const glm::vec4& aVector) noexcept;

std::ostream& operator<<(std::ostream& aStream, const glm::mat4& aMatrix) noexcept;

#endif
