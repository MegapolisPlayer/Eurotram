#include "Map.hpp"

void readLocationToString(std::ifstream& aStream, ObjectLocation& aLocation, const uint8_t aUnitsPerMeter) noexcept {
	std::string buffer;
	readBytesToString(aStream, buffer, 3*sizeof(uint32_t));
	aLocation.x = float(((uint32_t*)buffer.data())[0]) / aUnitsPerMeter;
	aLocation.y = float(((uint32_t*)buffer.data())[1]) / aUnitsPerMeter;
	aLocation.h = float(((uint32_t*)buffer.data())[2]) / aUnitsPerMeter;
}
void readLocationToString(std::ifstream& aStream, RotatedObjectLocation& aLocation, const uint8_t aUnitsPerMeter) noexcept {
	std::string buffer;
	readBytesToString(aStream, buffer, 4*sizeof(uint32_t));
	aLocation.x = float(((uint32_t*)buffer.data())[0]) / aUnitsPerMeter;
	aLocation.y = float(((uint32_t*)buffer.data())[1]) / aUnitsPerMeter;
	aLocation.h = float(((uint32_t*)buffer.data())[2]) / aUnitsPerMeter;
	aLocation.r = float(((uint32_t*)buffer.data())[3]) / aUnitsPerMeter;
}

Map::Map() noexcept
	: mTrackVertices(nullptr, 0, sizeof(Vertex)), mTrackIndices(nullptr, 0),
	mSwitchSignalModel("./SwitchSignal.glb"), mSignalModel("./Signal.glb"),
	mStationPillarModel("./Stop.glb"), mPillarMatrices(nullptr, 0), mPillarIds(nullptr, 0),
	mTreeModel("./Tree.glb"), mTreeMatrices(nullptr, 0), mTreeIds(nullptr, 0),
	mLightModel("./Streetlamp.glb"), mLightMatrices(nullptr, 0), mLightIds(nullptr, 0),
	mBuildingMatrices(nullptr, 0), mBuildingIds(nullptr, 0),
	mWallMatrices(nullptr, 0), mWallIds(nullptr, 0),
	mSignMatrices(nullptr, 0), mSignIds(nullptr, 0),
	mTexparcelVertices(nullptr, 0, sizeof(Vertex)), mTexparcelIndices(nullptr, 0) {}

Map::Map(const std::string_view aFilename) noexcept
	: mTrackVertices(nullptr, 0, sizeof(Vertex)), mTrackIndices(nullptr, 0),
	mSwitchSignalModel("./SwitchSignal.glb"), mSignalModel("./Signal.glb"),
	mStationPillarModel("./Stop.glb"), mPillarMatrices(nullptr, 0), mPillarIds(nullptr, 0),
	mTreeModel("./Tree.glb"), mTreeMatrices(nullptr, 0), mTreeIds(nullptr, 0),
	mLightModel("./Streetlamp.glb"), mLightMatrices(nullptr, 0), mLightIds(nullptr, 0),
	mBuildingMatrices(nullptr, 0), mBuildingIds(nullptr, 0),
	mWallMatrices(nullptr, 0), mWallIds(nullptr, 0),
	mSignMatrices(nullptr, 0), mSignIds(nullptr, 0),
	mTexparcelVertices(nullptr, 0, sizeof(Vertex)), mTexparcelIndices(nullptr, 0)
	{
	this->open(aFilename);
}

void Map::open(const std::string_view aFilename) noexcept {
	std::ifstream fileHandle;
	fileHandle.open(aFilename.data(), std::ios::binary | std::ios::in);
	if(!fileHandle.is_open()) {
		std::cerr << LogLevel::ERROR << "Failed to open map file " << aFilename << ".\n" << LogLevel::RESET;
		return;
	}

	std::string buffer;
	buffer.reserve(1024);

	//ETMP
	readBytesToString(fileHandle, buffer, 4);
	if(buffer != "ETMP") {
		std::cerr << LogLevel::ERROR << "File header wrong! Should be ETMP, instead got " << buffer << '\n' << LogLevel::RESET;
		return;
	}

	//V
	readBytesToString(fileHandle, buffer, 2);
	if((*(uint16_t*)buffer.data()) != MAP_FILE_FORMAT_VERSION) {
		std::cerr << LogLevel::ERROR << "File format version incompatible! Simulator version: " << MAP_FILE_FORMAT_VERSION << ", file version: " << (*(uint16_t*)buffer.data()) << '\n' << LogLevel::RESET;
		return;
	}

	//D
	readBytesToString(fileHandle, buffer, 8);
	std::cout << "Map file edit unix time: " << *((uint64_t*)buffer.data()) << '\n';

	//N
	readBytesToString(fileHandle, buffer, 4);
	uint32_t nodeAmount = *((uint32_t*)buffer.data());

	//I
	readBytesToString(fileHandle, buffer, 4);
	uint32_t switchAmount = *((uint32_t*)buffer.data());

	//T
	readBytesToString(fileHandle, buffer, 4);
	uint32_t trackAmount = *((uint32_t*)buffer.data());

	//Y
	readBytesToString(fileHandle, buffer, 4);
	uint32_t switchSignalAmount = *((uint32_t*)buffer.data());

	//S
	readBytesToString(fileHandle, buffer, 4);
	uint32_t signalAmount = *((uint32_t*)buffer.data());

	//R
	readBytesToString(fileHandle, buffer, 4);
	uint32_t radioboxAmount = *((uint32_t*)buffer.data());

	//P
	readBytesToString(fileHandle, buffer, 4);
	uint32_t pillarAmount = *((uint32_t*)buffer.data());

	//L
	readBytesToString(fileHandle, buffer, 4);
	uint32_t lampAmount = *((uint32_t*)buffer.data());

	//G
	readBytesToString(fileHandle, buffer, 4);
	uint32_t treeAmount = *((uint32_t*)buffer.data());

	//B
	readBytesToString(fileHandle, buffer, 4);
	uint32_t buildingAmount = *((uint32_t*)buffer.data());

	//M
	readBytesToString(fileHandle, buffer, 4);
	uint32_t landmarksAmount = *((uint32_t*)buffer.data());

	//W
	readBytesToString(fileHandle, buffer, 4);
	uint32_t wallAmount = *((uint32_t*)buffer.data());

	//F
	readBytesToString(fileHandle, buffer, 4);
	uint32_t signAmount = *((uint32_t*)buffer.data());

	//X
	readBytesToString(fileHandle, buffer, 4);
	uint32_t texparcelAmount = *((uint32_t*)buffer.data());

	//J...J
	std::getline(fileHandle, this->mMapName, '\0');
	std::cout << "Map name: " << this->mMapName << '\n';

	//A...A
	std::getline(fileHandle, this->mAuthorName, '\0');
	std::cout << "Author name: " << this->mAuthorName << '\n';

	//E
	readBytesToString(fileHandle, buffer, 1);
	uint8_t unitsPerMeter = *((uint8_t*)buffer.data());

	//Nodes list
	this->mNodes.reserve(nodeAmount);
	for(uint32_t i = 0; i < nodeAmount; i++) {
		this->mNodes.push_back({});
		readLocationToString(fileHandle, this->mNodes.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mNodes.back().code = *((uint32_t*)buffer.data());
	}

	//Switch list
	this->mSwitches.reserve(switchAmount);
	for(uint32_t i = 0; i < switchAmount; i++) {
		this->mSwitches.push_back({});
		readLocationToString(fileHandle, this->mSwitches.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().code = *((uint32_t*)buffer.data());

		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().before = *((uint32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().left = *((uint32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().front = *((uint32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().right = *((uint32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().radioBox = *((uint32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().signal = *((uint32_t*)buffer.data());

		readBytesToString(fileHandle, buffer, 1);
		this->mSwitches.back().letter = *((unsigned char*)buffer.data());
	}

	//Track and station track list
	this->mTracks.reserve(trackAmount);
	for(uint32_t i = 0; i < trackAmount; i++) {
		this->mTracks.push_back({});
		readBytesToString(fileHandle, buffer, 2);
		bool isStation = false;
		if(buffer == "ST") isStation = true;
		else if(buffer == "TK") isStation = false;
		else {
			std::cerr << LogLevel::ERROR << "Track header wrong!\n" << LogLevel::RESET;
			return;
		}

		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().first = *((uint32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().second = *((uint32_t*)buffer.data());

		//bezier stuff

		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().point1x = *((uint32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().point1y = *((uint32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().point2x = *((uint32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().point2y = *((uint32_t*)buffer.data());

		for(uint32_t i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
			readBytesToString(fileHandle, buffer, 4);
			this->mTracks.back().heightpoints[i] = *((uint32_t*)buffer.data());
		}

		if(isStation) {
			readBytesToString(fileHandle, buffer, 4);
			this->mTracks.back().code =  *((uint32_t*)buffer.data());
		}

	}

	//Switch signal list
	this->mSwitchSignals.reserve(switchSignalAmount);
	for(uint32_t i = 0; i < switchSignalAmount; i++) {
		this->mSwitchSignals.push_back({});
		readLocationToString(fileHandle, this->mSwitchSignals.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitchSignals.back().code =  *((uint32_t*)buffer.data());

		readBytesToString(fileHandle, buffer, 1);
		uint8_t amountLetters = *((uint8_t*)buffer.data());
		readBytesToString(fileHandle, buffer, amountLetters);
		this->mSwitchSignals.back().letters = buffer;
	}

	//Signal list
	this->mSignals.reserve(signalAmount);
	for(uint32_t i = 0; i < signalAmount; i++) {
		this->mSignals.push_back({});
		bool isPresignal = false;
		if(buffer == "PJ") isPresignal = true;
		else if(buffer == "JS") isPresignal = false;
		else {
			std::cerr << LogLevel::ERROR << "(Pre)signal header wrong!\n" << LogLevel::RESET;
			return;
		}
		readLocationToString(fileHandle, this->mSignals.back().location, unitsPerMeter);

		readBytesToString(fileHandle, buffer, 4);
		this->mSignals.back().poleHeight =  *((uint32_t*)buffer.data());

		if(isPresignal) {
			//I - id of signal from where to copy
			readBytesToString(fileHandle, buffer, 4);
			this->mSignals.back().signalCopyId =  *((uint32_t*)buffer.data());
		}
		else {
			this->mSignals.back().signalCopyId = -1;
		}

		readBytesToString(fileHandle, buffer, 4);
		this->mSignals.back().code =  *((uint32_t*)buffer.data());
	}

	//Radiobox list
	this->mRadioboxes.reserve(radioboxAmount);
	for(uint32_t i = 0; i < radioboxAmount; i++) {
		this->mRadioboxes.push_back({});
		readLocationToString(fileHandle, this->mRadioboxes.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().code = *((uint32_t*)buffer.data());
	}

	//Station pillar list
	this->mPillars.reserve(pillarAmount);
	for(uint32_t i = 0; i < pillarAmount; i++) {
		this->mPillars.push_back({});
		readLocationToString(fileHandle, this->mPillars.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().code = *((uint32_t*)buffer.data());
	}

	//Lightpost list
	this->mLights.reserve(lampAmount);
	for(uint32_t i = 0; i < lampAmount; i++) {
		this->mLights.push_back({});
		readLocationToString(fileHandle, this->mLights.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().code = *((uint32_t*)buffer.data());
	}

	//Tree list
	this->mTrees.reserve(treeAmount);
	for(uint32_t i = 0; i < treeAmount; i++) {
		this->mTrees.push_back({});
		readLocationToString(fileHandle, this->mTrees.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().code = *((uint32_t*)buffer.data());
	}

	//Building list
	this->mBuildings.reserve(buildingAmount);
	for(uint32_t i = 0; i < buildingAmount; i++) {
		this->mBuildings.push_back({});
		readLocationToString(fileHandle, this->mBuildings.back().location, unitsPerMeter);

		//type
		readBytesToString(fileHandle, buffer, 1);
		this->mBuildings.back().type = (BuildingType)*((uint8_t*)buffer.data());

		readBytesToString(fileHandle, buffer, 4);
		this->mBuildings.back().code = *((uint32_t*)buffer.data());
	}

	//Landmark list
	this->mLandmarks.reserve(landmarksAmount);
	for(uint32_t i = 0; i < landmarksAmount; i++) {
		this->mLandmarks.push_back({});
		readLocationToString(fileHandle, this->mLandmarks.back().location, unitsPerMeter);
	}

	//Wall list
	this->mWalls.reserve(wallAmount);
	for(uint32_t i = 0; i < wallAmount; i++) {
		this->mWalls.push_back({});
		readLocationToString(fileHandle, this->mWalls.back().firstPoint, unitsPerMeter);
		readLocationToString(fileHandle, this->mWalls.back().secondPoint, unitsPerMeter);

		readBytesToString(fileHandle, buffer, 2);
		this->mWalls.back().height = *((uint16_t*)buffer.data());

		readBytesToString(fileHandle, buffer, 4);
		this->mWalls.back().code = *((uint32_t*)buffer.data());

		std::getline(fileHandle, this->mWalls.back().material, '\0');
	}

	//Sign list
	this->mSigns.reserve(signAmount);
	for(uint32_t i = 0; i < signAmount; i++) {
		this->mSigns.push_back({});
		readLocationToString(fileHandle, this->mSigns.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSigns.back().code = *((uint32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 2);
		this->mSigns.back().type = (SignType)*((uint16_t*)buffer.data());
	}

	{
		//Texparcel list

		//generate texparcel vertices
		std::vector<Vertex> texparcelVertices;

		for(uint32_t i = 0; i < texparcelAmount; i++) {
			//texparcel has 4 vertices
			for(uint8_t j = 0; j < 4; j++) {
				readBytesToString(fileHandle, buffer, 12);
				texparcelVertices.push_back({});
				texparcelVertices.back().position.x = ((uint32_t*)buffer.data())[0];
				texparcelVertices.back().position.y = ((uint32_t*)buffer.data())[1];
				texparcelVertices.back().position.z = ((uint32_t*)buffer.data())[2];
			}
		}

		//generate texparcel indices
		std::vector<GLuint> texparcelIndices;
		for(uint32_t i = 0; i < texparcelVertices.size(); i+=4) {
			texparcelIndices.insert(texparcelIndices.end(), {i, 1+i, 2+i, 2+i, 3+i, i});
		}

		//set data
		this->mTexparcelVertices = VertexBuffer((float*)texparcelVertices.data(), texparcelVertices.size(), sizeof(Vertex));
		this->mTexparcelIndices = IndexBuffer(texparcelIndices.data(), texparcelIndices.size());
	}

	fileHandle.close();

	//print some info - amounts and author
	std::cout << "Nodes amount: " << this->mNodes.size() << '\n';
	std::cout << "Switch amount: " << this->mSwitches.size() << '\n';
	std::cout << "Track amount: " << this->mTracks.size() << '\n';
	std::cout << "Switch Signal amount: " << this->mSwitchSignals.size() << '\n';
	std::cout << "Signal amount: " << this->mSignals.size() << '\n';
	std::cout << "Radiobox amount: " << this->mRadioboxes.size() << '\n';
	std::cout << "Pillar amount: " << this->mPillars.size() << '\n';
	std::cout << "Tree amount: " << this->mTrees.size() << '\n';
	std::cout << "Streetlamp amount: " << this->mLights.size() << '\n';
	std::cout << "Building amount: " << this->mBuildings.size() << '\n';
	std::cout << "Landmark amount: " << this->mLandmarks.size() << '\n';
	std::cout << "Wall amount: " << this->mWalls.size() << '\n';
	std::cout << "Sign amount: " << this->mWalls.size() << '\n';
}

void Map::regenerateInstanceArray(StationCode aPrev, StationCode aCurrent, StationCode aNext, StationCode aAfterNext) noexcept {

}

void Map::draw() noexcept {
	this->mTexparcelIndices.draw();

	this->mBuildingIds;

	this->mTrackIndices.draw(); //draw track last
}

Track* Map::getStationByCode(std::string_view aCode) noexcept {
	for(Track& t : this->mTracks) {
		if(t.code == *(uint32_t*)aCode.data()) {
			return &t;
		}
	}
	return nullptr;
}

Map::~Map() noexcept {

}
