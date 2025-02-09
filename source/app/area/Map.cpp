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
		this->mNodes.back().code = *((uint8_t*)buffer.data());
	}

	//Switch list
	this->mSwitches.reserve(switchAmount);
	for(uint32_t i = 0; i < switchAmount; i++) {
		this->mSwitches.push_back({});
		readLocationToString(fileHandle, this->mSwitches.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().code = *((uint8_t*)buffer.data());
	}

	//Track and station track list
	this->mTracks.reserve(trackAmount);
	for(uint32_t i = 0; i < trackAmount; i++) {
		this->mTracks.push_back({});
		//TODO handle station tracks
	}

	//Switch signal list
	this->mSwitchSignals.reserve(switchSignalAmount);
	for(uint32_t i = 0; i < switchSignalAmount; i++) {
		this->mSwitchSignals.push_back({});
		readLocationToString(fileHandle, this->mSwitchSignals.back().location, unitsPerMeter);
	}

	//Signal list
	this->mSignals.reserve(signalAmount);
	for(uint32_t i = 0; i < signalAmount; i++) {
		this->mSignals.push_back({});
		//TODO handle presignals
		readLocationToString(fileHandle, this->mSignals.back().location, unitsPerMeter);

	}

	//Radiobox list
	this->mRadioboxes.reserve(radioboxAmount);
	for(uint32_t i = 0; i < radioboxAmount; i++) {
		this->mRadioboxes.push_back({});
		readLocationToString(fileHandle, this->mRadioboxes.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().code = *((uint8_t*)buffer.data());
	}

	//Station pillar list
	this->mPillars.reserve(pillarAmount);
	for(uint32_t i = 0; i < pillarAmount; i++) {
		this->mPillars.push_back({});
		readLocationToString(fileHandle, this->mPillars.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().code = *((uint8_t*)buffer.data());
	}

	//Lightpost list
	this->mLights.reserve(lampAmount);
	for(uint32_t i = 0; i < lampAmount; i++) {
		this->mLights.push_back({});
		readLocationToString(fileHandle, this->mLights.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().code = *((uint8_t*)buffer.data());
	}

	//Tree list
	this->mTrees.reserve(treeAmount);
	for(uint32_t i = 0; i < treeAmount; i++) {
		this->mTrees.push_back({});
		readLocationToString(fileHandle, this->mTrees.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().code = *((uint8_t*)buffer.data());
	}

	//Building list
	this->mBuildings.reserve(buildingAmount);
	for(uint32_t i = 0; i < buildingAmount; i++) {
		this->mBuildings.push_back({});
		readLocationToString(fileHandle, this->mBuildings.back().location, unitsPerMeter);
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
	}

	//Sign list
	this->mSigns.reserve(signAmount);
	for(uint32_t i = 0; i < signAmount; i++) {
		this->mSigns.push_back({});
		readLocationToString(fileHandle, this->mSigns.back().location, unitsPerMeter);
	}

	{
		//Texparcel list
		std::vector<Vertex> texparcelVertices;
		for(uint32_t i = 0; i < texparcelAmount; i++) {

		}

		//generate texparcel vertices

		//generate texparcel indices
		std::vector<GLuint> texparcelIndices;

		//set data
		this->mTexparcelVertices = VertexBuffer((float*)texparcelVertices.data(), texparcelVertices.size(), sizeof(Vertex));
		this->mTexparcelIndices = IndexBuffer(texparcelIndices.data(), texparcelIndices.size());
	}

	fileHandle.close();

	//print some info - amounts and author
}

void Map::regenerateInstanceArray(StationCode aPrev, StationCode aCurrent, StationCode aNext, StationCode aAfterNext) noexcept {

}

void Map::draw() noexcept {
	this->mTexparcelIndices.draw();

	this->mBuildingIds;

	this->mTrackIndices.draw(); //draw track last
}

Track& Map::getStationByCode(std::string_view aCode) const noexcept {
	for(std::unique_ptr<Track>& t : this->mTracks) {
		//
	}
}

Map::~Map() noexcept {

}
