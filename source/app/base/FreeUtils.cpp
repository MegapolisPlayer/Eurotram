#include "FreeUtils.hpp"

void setOutputColor(
	const ConsoleColor::ConsoleStreamColor aForeground,
	const ConsoleColor::ConsoleStreamColor aBackground
) noexcept {
#ifdef _WIN32
	static HANDLE sConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if(aForeground == ConsoleColor::DEFAULT) { SetConsoleTextAttribute(sConsoleHandle, 7); }
	else { SetConsoleTextAttribute(sConsoleHandle, (aBackground * 16) + aForeground); }
#endif
#ifdef __linux__
	std::string build;
	build.append("\x1b[0;");
	build.append(std::to_string(aForeground));
	build.push_back(';');
	build.append(std::to_string(aBackground + 10));
	build.push_back('m');
	std::cerr << build; //should also apply to cout and clog
#endif
}

std::ostream& operator<<(std::ostream& aStream, const LogLevel aLevel) noexcept {
	switch(aLevel) {
		case(LogLevel::OK):
			setOutputColor(ConsoleColor::WHITE, ConsoleColor::DEFAULT);
			return aStream;
		case(LogLevel::ERROR):
			setOutputColor(ConsoleColor::RED, ConsoleColor::DEFAULT);
			return aStream;
		case(LogLevel::WARNING):
			setOutputColor(ConsoleColor::YELLOW, ConsoleColor::DEFAULT);
			return aStream;
		case(LogLevel::SUCCESS):
			setOutputColor(ConsoleColor::LIGHT_GREEN, ConsoleColor::DEFAULT);
			return aStream;
		case(LogLevel::RESET):
			setOutputColor(ConsoleColor::DEFAULT, ConsoleColor::DEFAULT);
			return aStream;
	}
}

std::string readFile(std::fstream& aStream, const std::string_view aFilepath) noexcept {
	std::string result;
	std::string buffer(FILE_READ_BLOCK_SIZE, '\0');
	aStream.open(aFilepath.data(), std::ios::in | std::ios::binary);
	if(!aStream.is_open()) {
		std::cerr << LogLevel::ERROR << "Failed to open file " << aFilepath << "! \n" << LogLevel::RESET;
		return "";
	}

	while(aStream.read(&buffer[0], FILE_READ_BLOCK_SIZE)) {
		result.append(buffer);
		buffer.clear();
	};

	result.append(buffer);
	aStream.close();
	return result;
}

Timer::Timer() noexcept {}

void Timer::start() noexcept {
	this->mStart = std::chrono::system_clock::now();
}
void Timer::end() noexcept {
	this->mDuration = std::chrono::duration_cast<std::chrono::microseconds>((std::chrono::system_clock::now() - this->mStart));
}

uint64_t Timer::getMS() const noexcept {
	return std::chrono::duration_cast<std::chrono::milliseconds>(this->mDuration).count();
}
uint64_t Timer::getUS() const noexcept {
	return this->mDuration.count();
}

Timer::~Timer() noexcept {}
