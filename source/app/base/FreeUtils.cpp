#include "FreeUtils.hpp"

void setOutputColor(
	const ConsoleColor::ConsoleStreamColor aForeground,
	const ConsoleColor::ConsoleStreamColor aBackground
) noexcept {
#if defined(_WIN32)
	static HANDLE sConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if(aForeground == ConsoleColor::DEFAULT) { SetConsoleTextAttribute(sConsoleHandle, 7); }
	else { SetConsoleTextAttribute(sConsoleHandle, (aBackground * 16) + aForeground); }
#elif defined(__linux__)
	std::string build;
	build.append("\x1b[0;");
	build.append(std::to_string(aForeground));
	build.push_back(';');
	build.append(std::to_string(aBackground + 10));
	build.push_back('m');
	std::cerr << build; //should also apply to cout and clog
#else
	#error "Only Windows and Linux are supported!"
#endif
}

void setUTF8Encoding() noexcept {
#if defined(_WIN32)
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
#elif defined(__linux__)
	std::setlocale(LC_ALL, "en_US.UTF-8");
#else
	#error "Only Windows and Linux are supported!"
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
	result.reserve(FILE_READ_BLOCK_SIZE);
	std::string buffer(FILE_READ_BLOCK_SIZE, '\0');
	aStream.open(aFilepath.data(), std::ios::in | std::ios::binary);
	if(!aStream.is_open()) {
		std::cerr << LogLevel::ERROR << "Failed to open file " << aFilepath << "! \n" << LogLevel::RESET;
		return "";
	}

	while(aStream.read(&buffer[0], FILE_READ_BLOCK_SIZE)) {
		result.append(buffer);
		memset(&buffer[0], 0, FILE_READ_BLOCK_SIZE); //do not decrease size; string are contiguous
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
	this->mDuration = std::chrono::duration_cast<std::chrono::nanoseconds>((std::chrono::system_clock::now() - this->mStart));
}
void Timer::end(TimerAverage& aAvg) noexcept {
	this->mDuration = std::chrono::duration_cast<std::chrono::nanoseconds>((std::chrono::system_clock::now() - this->mStart));
	aAvg.mMicroseconds += this->getUS();
	aAvg.mAmount++;
}

uint64_t Timer::getMS() const noexcept {
	return this->mDuration.count()/1000000;
}
uint64_t Timer::getUS() const noexcept {
	return this->mDuration.count()/1000;
}
uint64_t Timer::getNS() const noexcept {
	return this->mDuration.count();
}

float Timer::getMSfloat() const noexcept {
	return this->mDuration.count()/1000000.0;
}
float Timer::getUSfloat() const noexcept {
	return this->mDuration.count()/1000.0;
}

Timer::~Timer() noexcept {}

TimerAverage::TimerAverage() noexcept
	:mMicroseconds(0), mAmount(0) {}

void TimerAverage::addTimer(const Timer& aTimer) noexcept {
	this->mMicroseconds += aTimer.getUS();
	this->mAmount++;
}

uint64_t TimerAverage::getAverageMS() const noexcept {
	return (this->mMicroseconds/1000)/this->mAmount;
}
float TimerAverage::getAverageMSfloat() const noexcept {
	return float(this->mMicroseconds/1000.0f)/float(this->mAmount);
}

uint64_t TimerAverage::getAverageUS() const noexcept {
	return this->mMicroseconds/this->mAmount;
}
float TimerAverage::getAverageUSfloat() const noexcept {
	return float(this->mMicroseconds)/float(this->mAmount);
}

TimerAverage::~TimerAverage() noexcept {}

void splitString(const std::string aString, const char aDelimiter, std::vector<std::string>& aOutputBuffer) noexcept {
	size_t previd = 0, id = 0;
	std::string buffer;
	while((id = aString.find(aDelimiter, id)) != std::string::npos) {
		buffer = aString.substr(previd, id-previd);
		aOutputBuffer.push_back(buffer);
		id++; //found char
		previd = id;
	}
	aOutputBuffer.push_back(aString.substr(previd)); //last part (from last delim)
}

void readBytesToString(std::ifstream& aStream, std::string& aBuffer, const uint64_t aBytes) noexcept {
	try {
		aBuffer.resize(aBytes);
		aStream.read(aBuffer.data(), aBytes);
	}
	catch(std::ios_base::failure f) {
		std::cerr << LogLevel::ERROR << "Error reading file!\n" << LogLevel::RESET;
		aBuffer.resize(0); //make buffer empty
	}
}

std::ostream& operator<<(std::ostream& aStream, const glm::vec2& aVector) noexcept {
	aStream << '[' << aVector.x << ';' << aVector.y << ']';
	return aStream;
}
std::ostream& operator<< (std::ostream& aStream, const glm::vec3& aVector) noexcept {
	aStream << '[' << aVector.x << ';' << aVector.y << ';' << aVector.z << ']';
	return aStream;
}
std::ostream& operator<<(std::ostream& aStream, const glm::vec4& aVector) noexcept {
	aStream << '[' << aVector.x << ';' << aVector.y << ';' << aVector.z << ';' << aVector.w << ']';
	return aStream;
}

std::ostream& operator<<(std::ostream& aStream, const glm::mat4& aMatrix) noexcept {
	aStream <<
	'[' << aMatrix[0][0] << ',' << aMatrix[0][1] << ',' << aMatrix[0][2] << ',' << aMatrix[0][3] << ']' << '\n' <<
	'|' << aMatrix[1][0] << ',' << aMatrix[1][1] << ',' << aMatrix[1][2] << ',' << aMatrix[1][3] << '|' << '\n' <<
	'|' << aMatrix[2][0] << ',' << aMatrix[2][1] << ',' << aMatrix[2][2] << ',' << aMatrix[2][3] << '|' << '\n' <<
	'[' << aMatrix[3][0] << ',' << aMatrix[3][1] << ',' << aMatrix[3][2] << ',' << aMatrix[3][3] << ']' << '\n';
	return aStream;
}
