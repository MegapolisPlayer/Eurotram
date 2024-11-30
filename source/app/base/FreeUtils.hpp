#ifndef EUROTRAM_FREEUTILS
#define EUROTRAM_FREEUTILS
#include "Setup.hpp"

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

class Timer {
public:
	Timer() noexcept;

	void start() noexcept;
	void end() noexcept;

	uint64_t getMS() const noexcept;
	uint64_t getUS() const noexcept;

	~Timer() noexcept;
private:
	std::chrono::system_clock::time_point mStart;
	std::chrono::system_clock::duration mDuration;
};

// OTHER

template<typename tComparableType>
concept Comparable = requires(tComparableType a, tComparableType b) {
	a == b;
};

#endif
