#include "Map.hpp"

void readLocationToString(std::ifstream& aStream, ObjectLocation& aLocation, const uint8_t aUnitsPerMeter) noexcept {
	std::string buffer;
	readBytesToString(aStream, buffer, 3*sizeof(int32_t));
	aLocation.x = float(((int32_t*)buffer.data())[0]) / aUnitsPerMeter;
	aLocation.y = float(((int32_t*)buffer.data())[1]) / aUnitsPerMeter;
	aLocation.h = float(((int32_t*)buffer.data())[2]) / aUnitsPerMeter;
}
void readLocationToString(std::ifstream& aStream, RotatedObjectLocation& aLocation, const uint8_t aUnitsPerMeter) noexcept {
	std::string buffer;
	readBytesToString(aStream, buffer, 3*sizeof(int32_t));
	aLocation.x = float(((int32_t*)buffer.data())[0]) / aUnitsPerMeter;
	aLocation.y = float(((int32_t*)buffer.data())[1]) / aUnitsPerMeter;
	aLocation.h = float(((int32_t*)buffer.data())[2]) / aUnitsPerMeter;
	//rotation only 2 bytes
	readBytesToString(aStream, buffer, sizeof(int16_t));
	aLocation.r = float(*((int16_t*)buffer.data()));
}

GMSEntry* Map::msTrackMaterial = nullptr;

Map::Map() noexcept :
	mOverheadWireVertices(nullptr, 0, 0), mOverheadWireIndices(nullptr, 0),
	mTrackVertices(nullptr, 0, 0), mTrackIndices(nullptr, 0),
	mSwitchSignalModel("./Switch.glb"), mSwitchSignalMatrices(nullptr, 0),
	mSignalModel("./Signal.glb"),  mSignalMatrices(nullptr, 0),
	mPoleModel("./Pole.glb"), mPresignalModel("./Presig.glb"), mPoleMatrices(nullptr, 0), mPresignalMatrices(nullptr, 0),
	mStationPillarModel("./Stop.glb"), mPillarMatrices(nullptr, 0),
	mTreeModel("./Tree.glb"), mTreeMatrices(nullptr, 0),
	mLightModel("./Streetlamp.glb"), mLightMatrices(nullptr, 0),
	mWallVertices(nullptr, 0, 0), mWallIndices(nullptr, 0), mFirstWallMaterial(0), mLastWallMaterial(0),
	mSignVertices(nullptr, 0, 0), mSignIndices(nullptr, 0), mFirstSignMaterial(0), mLastSignMaterial(0),
	mTexparcelVertices(nullptr, 0, 0), mTexparcelIndices(nullptr, 0) {}

Map::Map(const std::string_view aFilename) noexcept :
	mOverheadWireVertices(nullptr, 0, 0), mOverheadWireIndices(nullptr, 0),
	mTrackVertices(nullptr, 0, 0), mTrackIndices(nullptr, 0),
	mSwitchSignalModel("./Switch.glb"), mSwitchSignalMatrices(nullptr, 0),
	mSignalModel("./Signal.glb"),  mSignalMatrices(nullptr, 0),
	mPoleModel("./Pole.glb"), mPresignalModel("./Presig.glb"), mPoleMatrices(nullptr, 0), mPresignalMatrices(nullptr, 0),
	mStationPillarModel("./Stop.glb"), mPillarMatrices(nullptr, 0),
	mTreeModel("./Tree.glb"), mTreeMatrices(nullptr, 0),
	mLightModel("./Streetlamp.glb"), mLightMatrices(nullptr, 0),
	mWallVertices(nullptr, 0, 0), mWallIndices(nullptr, 0), mFirstWallMaterial(0), mLastWallMaterial(0),
	mSignVertices(nullptr, 0, 0), mSignIndices(nullptr, 0), mFirstSignMaterial(0), mLastSignMaterial(0),
	mTexparcelVertices(nullptr, 0, 0), mTexparcelIndices(nullptr, 0) {
	this->open(aFilename);
}

void Map::open(const std::string_view aFilename) noexcept {
	//load assets only once
	if(!this->mLoadedAssets) {
		//values for typical metal for track
		Map::msTrackMaterial = GlobalMaterialStore::add();
		Map::msTrackMaterial->material.specular = glm::vec4(0.5);
		Map::msTrackMaterial->material.ior = 1.0;
		Map::msTrackMaterial->material.shininess = 0.5;
		Map::msTrackMaterial->material.textureAmount = 1.0;
		Map::msTrackMaterial->material.textureSlot = 1;
		Map::msTrackMaterial->name = "TrackMaterial";
		Map::msTrackMaterial->path = "RailTexture.png";
		//we do need repeat here
		Map::msTrackMaterial->texture = Texture("RailTexture.png", true, TextureScale::LINEAR, TextureBorder::REPEAT);
		Map::msTrackMaterial->texture.setOutOfBoundsColor(0.5, 0.0, 0.0, 1.0);

		//load building models
		this->mBuildingModels.emplace_back(std::filesystem::path("./BuildingNormal.glb"));
		this->mBuildingModels.emplace_back(std::filesystem::path("./BuildingCorner.glb"));
		this->mBuildingModels.emplace_back(std::filesystem::path("./BuildingFamily.glb"));
		this->mBuildingModels.emplace_back(std::filesystem::path("./BuildingPrefab.glb"));

		//amount of types
		for(uint8_t i = 0; i < 4; i++) {
			this->mBuildingMatrices.emplace_back(ShaderBuffer(nullptr, 0));
			this->mBuildingCounts.emplace_back(0);
		}

		//other models loaded

		this->mLoadedAssets = true;
	}

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
	this->mSwitchSignalCount = *((uint32_t*)buffer.data());

	//S
	readBytesToString(fileHandle, buffer, 4);
	this->mSignalCount = *((uint32_t*)buffer.data());

	//R
	readBytesToString(fileHandle, buffer, 4);
	uint32_t radioboxAmount = *((uint32_t*)buffer.data());

	//P
	readBytesToString(fileHandle, buffer, 4);
	this->mPillarCount = *((uint32_t*)buffer.data());

	//L
	readBytesToString(fileHandle, buffer, 4);
	this->mLightCount = *((uint32_t*)buffer.data());

	//G
	readBytesToString(fileHandle, buffer, 4);
	this->mTreeCount = *((uint32_t*)buffer.data());

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

	//print header info - amounts and author
	std::cout << "Nodes amount: " << nodeAmount << '\n';
	std::cout << "Switch amount: " << switchAmount << '\n';
	std::cout << "Track amount: " << trackAmount << '\n';
	std::cout << "Switch Signal amount: " << this->mSwitchSignalCount << '\n';
	std::cout << "Signal amount: " << this->mSignalCount << '\n';
	std::cout << "Radiobox amount: " << radioboxAmount << '\n';
	std::cout << "Pillar amount: " << this->mPillarCount << '\n';
	std::cout << "Tree amount: " << this->mTreeCount << '\n';
	std::cout << "Streetlamp amount: " << this->mLightCount  << '\n';
	std::cout << "Building amount: " << buildingAmount << '\n';
	std::cout << "Landmark amount: " << landmarksAmount << '\n';
	std::cout << "Wall amount: " << wallAmount << '\n';
	std::cout << "Sign amount: " << signAmount << '\n';
	std::cout << "Texparcel amount: " << texparcelAmount << '\n';

	//Nodes list
	this->mNodes.reserve(nodeAmount);
	for(uint32_t i = 0; i < nodeAmount; i++) {
		this->mNodes.push_back({});
		readLocationToString(fileHandle, this->mNodes.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mNodes.back().code = *((uint32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mNodes.back().codeBorder = *((uint32_t*)buffer.data());
	}

	//Switch list
	this->mSwitches.reserve(switchAmount);
	for(uint32_t i = 0; i < switchAmount; i++) {
		this->mSwitches.push_back({});
		readLocationToString(fileHandle, this->mSwitches.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().code = *((uint32_t*)buffer.data());

		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().before = *((int32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().left = *((int32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().front = *((int32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().right = *((int32_t*)buffer.data());

		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().radioBox = *((int32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitches.back().signal = *((int32_t*)buffer.data());

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
			std::cerr << LogLevel::ERROR << "Track header no. " << i <<  " wrong! Got value: " << buffer << "\n" << LogLevel::RESET;
			return;
		}

		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().first = *((int32_t*)buffer.data());
		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().second = *((int32_t*)buffer.data());

		readBytesToString(fileHandle, buffer, 1);
		this->mTracks.back().flags = (TrackFlags)*((uint8_t*)buffer.data());

		//bezier stuff

		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().point1x = *((int32_t*)buffer.data()) / unitsPerMeter;
		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().point1y = *((int32_t*)buffer.data()) / unitsPerMeter;
		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().point2x = *((int32_t*)buffer.data()) / unitsPerMeter;
		readBytesToString(fileHandle, buffer, 4);
		this->mTracks.back().point2y = *((int32_t*)buffer.data()) / unitsPerMeter;

		for(uint32_t i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
			readBytesToString(fileHandle, buffer, 4);
			//no units per meter division here - stored as multiple of 100
			this->mTracks.back().heightpoints[i] = (*((int32_t*)buffer.data())) / 100.0;
		}

		if(isStation) {
			readBytesToString(fileHandle, buffer, 4);
			this->mTracks.back().code =  *((uint32_t*)buffer.data());
		}
	}

	//Switch signal list
	this->mSwitchSignals.reserve(this->mSwitchSignalCount);
	for(uint32_t i = 0; i < this->mSwitchSignalCount; i++) {
		this->mSwitchSignals.push_back({});
		readLocationToString(fileHandle, this->mSwitchSignals.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mSwitchSignals.back().code =  *((uint32_t*)buffer.data());

		readBytesToString(fileHandle, buffer, 1);
		uint8_t amountLetters = *((uint8_t*)buffer.data());
		readBytesToString(fileHandle, buffer, amountLetters);
		this->mSwitchSignals.back().letters = buffer;
	}

	//Signal (and pre-signal) list
	this->mSignals.reserve(this->mSignalCount);
	for(uint32_t i = 0; i < this->mSignalCount; i++) {
		this->mSignals.push_back({});
		bool isPresignal = false;
		readBytesToString(fileHandle, buffer, 2);
		if(buffer == "PJ") isPresignal = true;
		else if(buffer == "JS") isPresignal = false;
		else {
			std::cerr << LogLevel::ERROR << "(Pre)signal header wrong! - " << buffer << "\n" << LogLevel::RESET;
			return;
		}
		readLocationToString(fileHandle, this->mSignals.back().location, unitsPerMeter);

		readBytesToString(fileHandle, buffer, 2);
		this->mSignals.back().poleHeight =  *((int16_t*)buffer.data());

		if(isPresignal) {
			//I - id of signal from where to copy
			readBytesToString(fileHandle, buffer, 4);
			this->mSignals.back().signalCopyId =  *((int32_t*)buffer.data());
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
		this->mRadioboxes.back().code = *((uint32_t*)buffer.data());
	}

	//Station pillar list
	this->mPillars.reserve(this->mPillarCount);
	for(uint32_t i = 0; i < this->mPillarCount; i++) {
		this->mPillars.push_back({});
		readLocationToString(fileHandle, this->mPillars.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mPillars.back().code = *((uint32_t*)buffer.data());
	}

	//Lightpost list
	this->mLights.reserve(this->mLightCount);
	for(uint32_t i = 0; i < this->mLightCount; i++) {
		this->mLights.push_back({});
		readLocationToString(fileHandle, this->mLights.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mLights.back().code = *((uint32_t*)buffer.data());
	}

	//Tree list
	this->mTrees.reserve(this->mTreeCount);
	for(uint32_t i = 0; i < this->mTreeCount; i++) {
		this->mTrees.push_back({});
		readLocationToString(fileHandle, this->mTrees.back().location, unitsPerMeter);
		readBytesToString(fileHandle, buffer, 4);
		this->mTrees.back().code = *((uint32_t*)buffer.data());
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
		readBytesToString(fileHandle, buffer, 4);
		this->mLandmarks.back().landmarkCode = *(uint32_t*)buffer.data();
	}

	{
		//Wall list

		std::vector<Vertex> wallVertices;

		this->mFirstWallMaterial = GlobalMaterialStore::getLength();
		for(uint32_t i = 0; i < wallAmount; i++) {
			ObjectLocation o1;
			ObjectLocation o2;

			readLocationToString(fileHandle, o1, unitsPerMeter);
			readLocationToString(fileHandle, o2, unitsPerMeter);

			readBytesToString(fileHandle, buffer, 2);
			int16_t height = *((int16_t*)buffer.data());

			readBytesToString(fileHandle, buffer, 4); //ignore st, code

			std::getline(fileHandle, buffer, '\0'); //material name

			//front face
			wallVertices.emplace_back(glm::vec3(o1.x, o1.h, o1.y));
			wallVertices.emplace_back(glm::vec3(o2.x, o2.h, o2.y));
			wallVertices.emplace_back(glm::vec3(o2.x, o2.h+height, o2.y));
			wallVertices.emplace_back(glm::vec3(o1.x, o1.h+height, o1.y));

			//already CCW order
			glm::vec3 frontNormal = Math::normals(
				wallVertices[wallVertices.size()-4].position,
				wallVertices[wallVertices.size()-3].position,
				wallVertices[wallVertices.size()-2].position,
				wallVertices[wallVertices.size()-1].position
			);
			for(int8_t j = 0; j < 4; j++) {
				wallVertices[wallVertices.size()-4+j].normal = frontNormal;
			}

			//back face - we need this due to backface culling
			wallVertices.emplace_back(glm::vec3(o2.x, o2.h, o2.y));
			wallVertices.emplace_back(glm::vec3(o1.x, o1.h, o1.y));
			wallVertices.emplace_back(glm::vec3(o1.x, o1.h+height, o1.y));
			wallVertices.emplace_back(glm::vec3(o2.x, o2.h+height, o2.y));

			//already CCW order
			glm::vec3 backNormal = Math::normals(
				wallVertices[wallVertices.size()-4].position,
				wallVertices[wallVertices.size()-3].position,
				wallVertices[wallVertices.size()-2].position,
				wallVertices[wallVertices.size()-1].position
			);
			for(int8_t j = 0; j < 4; j++) {
				wallVertices[wallVertices.size()-4+j].normal = backNormal;
			}

			//tex coords - in correct CCW order from bottom left
			wallVertices[wallVertices.size()-8].texCoords = glm::vec2(0.0, 0.0);
			wallVertices[wallVertices.size()-7].texCoords = glm::vec2(1.0, 0.0);
			wallVertices[wallVertices.size()-6].texCoords = glm::vec2(1.0, 1.0);
			wallVertices[wallVertices.size()-5].texCoords = glm::vec2(0.0, 1.0);

			wallVertices[wallVertices.size()-4].texCoords = glm::vec2(0.0, 0.0);
			wallVertices[wallVertices.size()-3].texCoords = glm::vec2(1.0, 0.0);
			wallVertices[wallVertices.size()-2].texCoords = glm::vec2(1.0, 1.0);
			wallVertices[wallVertices.size()-1].texCoords = glm::vec2(0.0, 1.0);

			//WallMat is prefix for texparcels
			GMSEntry* wallMaterial = GlobalMaterialStore::getByName("WallMat."+buffer);
			uint64_t wallMaterialId = 0;
			if(!wallMaterial) {
				wallMaterial = GlobalMaterialStore::add(&wallMaterialId);
				this->mWallMaterials.push_back(wallMaterial);
				wallMaterial->name = "WallMat."+buffer;
				wallMaterial->texture = Texture(WALL_TEXTURE_PREFIX+buffer+"."+TEXTURE_EXTENSION);
				wallMaterial->material.textureSlot = wallMaterialId - this->mFirstWallMaterial;
				wallVertices[wallVertices.size()-4].materialId = wallMaterialId - this->mFirstWallMaterial;
				wallVertices[wallVertices.size()-3].materialId = wallMaterialId - this->mFirstWallMaterial;
				wallVertices[wallVertices.size()-2].materialId = wallMaterialId - this->mFirstWallMaterial;
				wallVertices[wallVertices.size()-1].materialId = wallMaterialId - this->mFirstWallMaterial;
			}
			else {
				wallMaterialId = GlobalMaterialStore::findId("WallMat."+buffer); //variants will be add later
				wallVertices[wallVertices.size()-4].materialId = wallMaterialId - this->mFirstWallMaterial;
				wallVertices[wallVertices.size()-3].materialId = wallMaterialId - this->mFirstWallMaterial;
				wallVertices[wallVertices.size()-2].materialId = wallMaterialId - this->mFirstWallMaterial;
				wallVertices[wallVertices.size()-1].materialId = wallMaterialId - this->mFirstWallMaterial;
			}
		}
		this->mLastWallMaterial = GlobalMaterialStore::getLength(); //non inclusive

		std::vector<GLuint> wallIndices;
		//same for both sides of wall
		for(uint32_t i = 0; i < wallVertices.size(); i+=4) {
			wallIndices.insert(wallIndices.end(), {i, 1+i, 2+i, 2+i, 3+i, 0+i});
		}

		this->mWallArray.bind();

		this->mWallVertices = VertexBuffer((float*)wallVertices.data(), wallVertices.size(), STANDARD_MODEL_VERTEX_FLOAT_AMOUNT);
		this->mWallVertices.enableStandardAttributes(&this->mWallArray);

		this->mWallIndices = IndexBuffer(wallIndices.data(), wallIndices.size());
		this->mWallIndices.bind();
	}

	{
		//Sign list

		std::vector<Vertex> signVertices;
		std::vector<GLuint> signIndices;

		this->mSigns.reserve(signAmount);
		for(uint32_t i = 0; i < signAmount; i++) {
			this->mSigns.push_back({});
			readLocationToString(fileHandle, this->mSigns.back().location, unitsPerMeter);
			readBytesToString(fileHandle, buffer, 4);
			this->mSigns.back().code = *((uint32_t*)buffer.data());
			readBytesToString(fileHandle, buffer, 2);
			this->mSigns.back().type = (SignType)*((int16_t*)buffer.data());

			//generate vertices

			//TODO

			//generate indices
		}

		this->mSignArray.bind();

		this->mSignVertices = VertexBuffer((float*)signVertices.data(), signVertices.size(), STANDARD_MODEL_VERTEX_FLOAT_AMOUNT);
		this->mSignVertices.enableStandardAttributes(&this->mSignArray);

		this->mSignIndices = IndexBuffer(signIndices.data(), signIndices.size());
		this->mSignIndices.bind();
	}

	{
		//Texparcel list

		//generate texparcel vertices
		std::vector<Vertex> texparcelVertices;

		this->mFirstTPMaterial = GlobalMaterialStore::getLength();
		for(uint32_t i = 0; i < texparcelAmount; i++) {
			//texparcel has 4 vertices - convert from clockwise in file to CCW in GL
			readBytesToString(fileHandle, buffer, 48); //3 values * 4 bytes * 4 points - 12 values
			assert(buffer.size() == 48);

			for(int8_t j = 3; j >= 0; j--) {
				texparcelVertices.push_back({});
				texparcelVertices.back().position.x = (((int32_t*)buffer.data())[j*3+0]) / float(unitsPerMeter); //x
				texparcelVertices.back().position.y = (((int32_t*)buffer.data())[j*3+2]) / float(unitsPerMeter); //h height
				texparcelVertices.back().position.z = (((int32_t*)buffer.data())[j*3+1]) / float(unitsPerMeter); //y
			}
			//already CCW order
			glm::vec3 normal = Math::normals(
				texparcelVertices[texparcelVertices.size()-4].position,
				texparcelVertices[texparcelVertices.size()-3].position,
				texparcelVertices[texparcelVertices.size()-2].position,
				texparcelVertices[texparcelVertices.size()-1].position
			);

			//normals (set same for all)
			for(int8_t j = 0; j < 4; j++) {
				texparcelVertices[texparcelVertices.size()-4+j].normal = normal;
			}

			//tex coords - in correct CCW order from bottom left
			texparcelVertices[texparcelVertices.size()-4].texCoords = glm::vec2(0.0, 0.0);
			texparcelVertices[texparcelVertices.size()-3].texCoords = glm::vec2(1.0, 0.0);
			texparcelVertices[texparcelVertices.size()-2].texCoords = glm::vec2(1.0, 1.0);
			texparcelVertices[texparcelVertices.size()-1].texCoords = glm::vec2(0.0, 1.0);

			readBytesToString(fileHandle, buffer, 8); //2 station codes
			std::getline(fileHandle, buffer, '\0'); //material name

			//TexparcelMat is prefix for texparcels
			GMSEntry* tpMaterial = GlobalMaterialStore::getByName("TexparcelMat."+buffer);
			uint64_t texparcelMaterialId = 0;
			if(!tpMaterial) {
				tpMaterial = GlobalMaterialStore::add(&texparcelMaterialId);
				this->mTexparcelMaterials.push_back(tpMaterial);
				tpMaterial->name = "TexparcelMat."+buffer;
				tpMaterial->texture = Texture(TEXPARCEL_TEXTURE_PREFIX+buffer+"."+TEXTURE_EXTENSION);
				tpMaterial->material.textureSlot = texparcelMaterialId - this->mFirstTPMaterial;
				texparcelVertices[texparcelVertices.size()-4].materialId = texparcelMaterialId - this->mFirstTPMaterial;
				texparcelVertices[texparcelVertices.size()-3].materialId = texparcelMaterialId - this->mFirstTPMaterial;
				texparcelVertices[texparcelVertices.size()-2].materialId = texparcelMaterialId - this->mFirstTPMaterial;
				texparcelVertices[texparcelVertices.size()-1].materialId = texparcelMaterialId - this->mFirstTPMaterial;
			}
			else {
				texparcelMaterialId = GlobalMaterialStore::findId("TexparcelMat."+buffer); //variants will be add later
				texparcelVertices[texparcelVertices.size()-4].materialId = texparcelMaterialId - this->mFirstTPMaterial;
				texparcelVertices[texparcelVertices.size()-3].materialId = texparcelMaterialId - this->mFirstTPMaterial;
				texparcelVertices[texparcelVertices.size()-2].materialId = texparcelMaterialId - this->mFirstTPMaterial;
				texparcelVertices[texparcelVertices.size()-1].materialId = texparcelMaterialId - this->mFirstTPMaterial;
			}
		}

		this->mLastTPMaterial = GlobalMaterialStore::getLength();

		//generate texparcel indices
		std::vector<GLuint> texparcelIndices;
		for(uint32_t i = 0; i < texparcelVertices.size(); i+=4) {
			texparcelIndices.insert(texparcelIndices.end(), {i, 1+i, 2+i, 2+i, 3+i, 0+i});
		}

		this->mTexparcelArray.bind();

		std::cout << "Map texparcel vertices: " << texparcelVertices.size() << ", indices: " << texparcelIndices.size() << '\n';

		//set data
		this->mTexparcelVertices = VertexBuffer((float*)texparcelVertices.data(), texparcelVertices.size(), STANDARD_MODEL_VERTEX_FLOAT_AMOUNT);
		this->mTexparcelVertices.enableStandardAttributes(&this->mTexparcelArray);

		this->mTexparcelIndices = IndexBuffer(texparcelIndices.data(), texparcelIndices.size());
		this->mTexparcelIndices.bind();
	}

	fileHandle.close();

	//generate track points

	{
		std::vector<Vertex> trackVertices;
		std::vector<GLuint> trackIndices;

		for(uint32_t trackId = 0; trackId < this->mTracks.size(); trackId++) {
			Track& t = this->mTracks[trackId];

			glm::vec2 firstNode;
			glm::vec2 secondNode;

			if(t.flags & TRACK_FLAG_FIRST_SWITCH) {
				firstNode = glm::vec2(this->mSwitches[t.first].location.x, this->mSwitches[t.first].location.y);
			}
			else {
				firstNode = glm::vec2(this->mNodes[t.first].location.x, this->mNodes[t.first].location.y);
			}

			if(t.flags & TRACK_FLAG_SECOND_SWITCH){
				secondNode = glm::vec2(this->mSwitches[t.second].location.x, this->mSwitches[t.second].location.y);
			}
			else {
				secondNode = glm::vec2(this->mNodes[t.second].location.x, this->mNodes[t.second].location.y);
			}

			uint64_t oldVertexAmount = trackVertices.size();
			if(t.flags & TRACK_FLAG_BEZIER) {
				trackVertices.reserve(trackVertices.size() + Math::DEFAULT_BEZIER_PRECISION*4);
				trackIndices.reserve(trackIndices.size() + Math::DEFAULT_BEZIER_PRECISION/4*6); //for every 4 vertices 6 indices

				std::vector<Math::BezierPoint> curve = Math::bezier(
					firstNode,
					glm::vec2(t.point1x, t.point1y),
					glm::vec2(t.point2x, t.point2y),
					secondNode
				);

				t.points = curve;
				t.length = Math::bezierLength(t.points);
				t.pointsAmount = t.points.size();

				std::vector<Math::BezierPoint> topCurve = Math::moveBezier(curve, glm::vec2(TRACK_GAUGE/2+TRACK_WIDTH/2));
				std::vector<Math::BezierPoint> topInnerCurve = Math::moveBezier(curve, glm::vec2(TRACK_GAUGE/2));

				std::vector<Math::BezierPoint> bottomCurve = Math::moveBezier(curve, glm::vec2(-TRACK_GAUGE/2-TRACK_WIDTH/2));
				std::vector<Math::BezierPoint> bottomInnerCurve = Math::moveBezier(curve, glm::vec2(-TRACK_GAUGE/2));

				//add in groups of 4 - quads
				//2 from each bezier curve
				//generate texcoords based on this

				for(uint32_t i = 0; i < topCurve.size(); i++) {
					uint8_t point1 = std::floor((float)i/(float)topCurve.size()*TRACK_HEIGHTPOINTS_AMOUNT);
					uint8_t point2 = std::ceil((float)i/(float)topCurve.size()*TRACK_HEIGHTPOINTS_AMOUNT);
					float weight = (float)i/(float)topCurve.size()*TRACK_HEIGHTPOINTS_AMOUNT - point1;
					float height = Math::linearInterpolation(t.heightpoints[point1], t.heightpoints[point2], weight)+TRACK_Z_FIGHT_AVOIDANCE;

					//H values are Y axis

					//rail 1
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(topCurve[i].x, height, topCurve[i].y);
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(topInnerCurve[i].x, height, topInnerCurve[i].y);

					//rail 2
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(bottomCurve[i].x, height, bottomCurve[i].y);
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(bottomInnerCurve[i].x, height, bottomInnerCurve[i].y);
				} //end of loop
			}
			else {
				t.length = glm::distance(firstNode, secondNode);
				t.pointsAmount = TRACK_HEIGHTPOINTS_AMOUNT;

				glm::vec2 dirVector = Math::getPerpendicularVectorFromPoints(firstNode, secondNode);
				glm::vec2 gaugeVector = dirVector*(TRACK_GAUGE/2);
				glm::vec2 gaugeLongVector = dirVector*(TRACK_GAUGE/2+TRACK_WIDTH/2);

				for(uint8_t i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
					//just linear points - same order
					//interpolate height

					glm::vec2 position = Math::linearInterpolation(firstNode, secondNode, i/float(TRACK_HEIGHTPOINTS_AMOUNT-1));
					float height = t.heightpoints[i]+TRACK_Z_FIGHT_AVOIDANCE;

					t.points.push_back(position);

					//rail 1
					trackVertices.emplace_back(glm::vec3(position.x+gaugeLongVector.x, height, position.y+gaugeLongVector.y));
					trackVertices.emplace_back(glm::vec3(position.x+gaugeVector.x, height, position.y+gaugeVector.y));

					//rail 2
					trackVertices.emplace_back(glm::vec3(position.x-gaugeVector.x, height, position.y-gaugeVector.y));
					trackVertices.emplace_back(glm::vec3(position.x-gaugeLongVector.x, height, position.y-gaugeLongVector.y));
				}
			}

			//add indices and tex coords - only for current track
			for(uint32_t i = oldVertexAmount; i < trackVertices.size()-4; i+=4) {
				//index buffer
				//[0,1,4 ; 1,5,4] top
				//[2,3,6 ; 3,7,6] bottom
				trackIndices.insert(trackIndices.end(), {
					i + 0,
					i + 1,
					i + 4,
					i + 1,
					i + 5,
					i + 4,
					i + 2,
					i + 3,
					i + 6,
					i + 3,
					i + 7,
					i + 6,
				});

				trackVertices[i+0].texCoords = glm::vec2(float(i+0)/t.pointsAmount, 1.0);
				trackVertices[i+1].texCoords = glm::vec2(float(i+1)/t.pointsAmount, 0.0);
				trackVertices[i+2].texCoords = glm::vec2(float(i+2)/t.pointsAmount, 0.0);
				trackVertices[i+3].texCoords = glm::vec2(float(i+3)/t.pointsAmount, 1.0);
			}
		}

		//add normals
		for(uint32_t i = 0; i < trackVertices.size(); i+=8) {
			//rail 1
			glm::vec3 normalTop = Math::normals(
				trackVertices[i+1].position,
				trackVertices[i+0].position,
				trackVertices[i+5].position,
				trackVertices[i+4].position
			);
			//rail 2
			glm::vec3 normalButton = Math::normals(
				trackVertices[i+3].position,
				trackVertices[i+7].position,
				trackVertices[i+6].position,
				trackVertices[i+2].position
			);

			trackVertices[i+0].normal = normalTop;
			trackVertices[i+1].normal = normalTop;
			trackVertices[i+2].normal = normalTop;
			trackVertices[i+3].normal = normalTop;
			trackVertices[i+4].normal = normalButton;
			trackVertices[i+5].normal = normalButton;
			trackVertices[i+6].normal = normalButton;
			trackVertices[i+7].normal = normalButton;
		}

		this->mTrackArray.bind();

		std::cout << "Map track vertices: " << trackVertices.size() << ", indices: " << trackIndices.size() << '\n';

		this->mTrackVertices = VertexBuffer((GLfloat*)trackVertices.data(), trackVertices.size(), STANDARD_MODEL_VERTEX_FLOAT_AMOUNT);
		this->mTrackVertices.enableStandardAttributes(&this->mTrackArray);

		this->mTrackIndices = IndexBuffer((GLuint*)trackIndices.data(), trackIndices.size());
		this->mTrackIndices.bind();
	}
}

void Map::randomizeBuildingColors() noexcept {
	//TODO
}

void Map::regenerateInstanceArray(StationCode aPrev, StationCode aCurrent, StationCode aNext, StationCode aAfterNext) noexcept {
	//switch signals
	{
		std::vector<glm::mat4> switchSignalInstanceTranslations;
		for(SwitchSignal& s : this->mSwitchSignals) {
			glm::mat4 translation = glm::translate(glm::mat4(1.0), glm::vec3(s.location.x, s.location.h, s.location.y));
			translation = glm::rotate(translation, glm::radians(s.location.r), glm::vec3(0.0, 1.0, 0.0));
			switchSignalInstanceTranslations.push_back(translation);
		}
		this->mSwitchSignalMatrices.setNewData(switchSignalInstanceTranslations.data(), switchSignalInstanceTranslations.size()*sizeof(glm::mat4));
	}

	//signals
	{
		this->mPoleCount = 0;
		this->mPresignalCount = 0;

		std::vector<glm::mat4> poleMatrices;
		std::vector<glm::mat4> signalInstanceTranslations;
		std::vector<glm::mat4> presignalMatrices;

		for(Signal& s : this->mSignals) {
			glm::mat4 translation = glm::translate(glm::mat4(1.0), glm::vec3(s.location.x, s.location.h+s.poleHeight-1, s.location.y));
			translation = glm::rotate(translation, glm::radians(s.location.r), glm::vec3(0.0, 1.0, 0.0));
			if(s.signalCopyId != -1) {
				presignalMatrices.push_back(translation);
				this->mPresignalCount++;
			}
			signalInstanceTranslations.push_back(translation);
			if(s.poleHeight > 0) {
				//without poleheight addition (but half because we scale from center)
				translation = glm::translate(glm::mat4(1.0), glm::vec3(s.location.x, s.location.h, s.location.y));
				translation = glm::rotate(translation, glm::radians(s.location.r), glm::vec3(0.0, 1.0, 0.0));
				translation = glm::scale(translation, glm::vec3(1.0, s.poleHeight, 1.0));
				poleMatrices.push_back(translation);
				this->mPoleCount++;
			}
		}

		this->mPoleMatrices.setNewData(poleMatrices.data(), poleMatrices.size()*sizeof(glm::mat4));
		this->mSignalMatrices.setNewData(signalInstanceTranslations.data(), signalInstanceTranslations.size()*sizeof(glm::mat4));
		this->mPresignalMatrices.setNewData(presignalMatrices.data(), presignalMatrices.size()*sizeof(glm::mat4));
	}

	//pillars
	{
		std::vector<glm::mat4> pillarInstanceTranslations;
		for(StationPillar& p : this->mPillars) {
			glm::mat4 translation = glm::translate(glm::mat4(1.0), glm::vec3(p.location.x, p.location.h, p.location.y));
			translation = glm::rotate(translation, glm::radians(p.location.r), glm::vec3(0.0, 1.0, 0.0));
			pillarInstanceTranslations.push_back(translation);
		}
		this->mPillarMatrices.setNewData(pillarInstanceTranslations.data(), pillarInstanceTranslations.size()*sizeof(glm::mat4));
	}

	//trees
	{
		std::uniform_real_distribution<> randomDistribution(0, 360);
		std::vector<glm::mat4> treeInstanceTranslations;
		for(Tree& t : this->mTrees) {
			glm::mat4 translation = glm::translate(glm::mat4(1.0), glm::vec3(t.location.x, t.location.h, t.location.y));
			translation = glm::rotate(translation, glm::radians((float)-randomDistribution(Math::getRandomGenerator())), glm::vec3(0.0, 1.0, 0.0));
			treeInstanceTranslations.push_back(translation);
		}
		this->mTreeMatrices.setNewData(treeInstanceTranslations.data(), treeInstanceTranslations.size()*sizeof(glm::mat4));
	}

	//light
	{
		std::vector<glm::mat4> lightInstanceTranslations;
		for(Lightpole& l : this->mLights) {
			glm::mat4 translation = glm::translate(glm::mat4(1.0), glm::vec3(l.location.x, l.location.h, l.location.y));
			translation = glm::rotate(translation, glm::radians(-l.location.r), glm::vec3(0.0, 1.0, 0.0));
			lightInstanceTranslations.push_back(translation);
		}
		this->mLightMatrices.setNewData(lightInstanceTranslations.data(), lightInstanceTranslations.size()*sizeof(glm::mat4));
	}

	//buildings - split per type
	{
		std::vector<std::vector<glm::mat4>> buildingInstanceTranslations;
		buildingInstanceTranslations.resize(4); //4 building types

		for(uint8_t i = 0; i < 4; i++) {
			buildingInstanceTranslations[i].clear();
			this->mBuildingCounts[i] = 0;
		}

		for(Building& b : this->mBuildings) {
			//TODO
			//if(b.code == aPrev || b.code == aCurrent || b.code == aNext || b.code == aAfterNext) {
				glm::mat4 translation = glm::translate(glm::mat4(1.0), glm::vec3(b.location.x, b.location.h, b.location.y));
				translation = glm::rotate(translation, glm::radians(-b.location.r), glm::vec3(0.0, 1.0, 0.0));
				buildingInstanceTranslations[(uint8_t)b.type].push_back(translation);
				this->mBuildingCounts[(uint8_t)b.type]++;
			//}
		}
		for(uint8_t i = 0; i < 4; i++) {
			this->mBuildingMatrices[i].setNewData(buildingInstanceTranslations[i].data(), buildingInstanceTranslations[i].size()*sizeof(glm::mat4));
		}
	}
}

void Map::updateTextures(StationCode aPrev, StationCode aCurrent, StationCode aNext, StationCode aAfterNext) noexcept {
	//signals
	for(Signal& s : this->mSignals) {
		s.updateTexture();
	}

	//switch signals
	for(SwitchSignal& s : this->mSwitchSignals) {
		s.updateTexture();
	}

	//pillars
	for(StationPillar& p : this->mPillars) {
		p.updateTexture();
	}
}

void Map::draw(UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices, const uint64_t aInstanceBufferLocation, UniformInt& aBoolStateUniform, UniformMat4* aTransformUniform, UniformMat3* aNormalUniform) noexcept {
	//ground (texparcels)

	if(aTransformUniform) {
		aTransformUniform->set(glm::mat4(1.0f));
	}
	if(aNormalUniform) {
		aNormalUniform->set(glm::transpose(glm::inverse(glm::mat4(1.0f))));
	}

	//no materials -> no texparcels!
	if(this->mTexparcelMaterials.size() > 0) {
		this->mTexparcelArray.bind();
		GlobalMaterialStore::copyDataToUniform(aUniform, this->mFirstTPMaterial, this->mLastTPMaterial);
		this->mTexparcelVertices.bind();
		this->mTexparcelIndices.bind();
		//this->mTexparcelVertices.drawPoints();
		this->mTexparcelIndices.draw();
	}

	//walls
	if(this->mWallMaterials.size()  > 0) {
		this->mWallArray.bind();
		GlobalMaterialStore::copyDataToUniform(aUniform, this->mFirstWallMaterial, this->mLastWallMaterial);
		this->mWallVertices.bind();
		this->mWallIndices.bind();
		//this->mWallVertices.drawPoints();
		this->mWallIndices.draw();
	}

	//all objects
	aBoolStateUniform.set(1);

	//signals
	this->mSignalMatrices.bind(aInstanceBufferLocation);
	this->mSignalModel.drawInstanced(aUniform, aBoneMatrices, this->mSignalCount, aTransformUniform, aNormalUniform);
	this->mPoleMatrices.bind(aInstanceBufferLocation);
	this->mPoleModel.drawInstanced(aUniform, aBoneMatrices, this->mPoleCount, aTransformUniform, aNormalUniform);
	this->mPresignalMatrices.bind(aInstanceBufferLocation);
	this->mPresignalModel.drawInstanced(aUniform, aBoneMatrices, this->mPresignalCount, aTransformUniform, aNormalUniform);

	//switch signals
	this->mSwitchSignalMatrices.bind(aInstanceBufferLocation);
	this->mSwitchSignalModel.drawInstanced(aUniform, aBoneMatrices, this->mSwitchSignalCount, aTransformUniform, aNormalUniform);

	//pillars
	this->mPillarMatrices.bind(aInstanceBufferLocation);
	this->mStationPillarModel.drawInstanced(aUniform, aBoneMatrices, this->mPillarCount, aTransformUniform, aNormalUniform);

	//trees
	this->mTreeMatrices.bind(aInstanceBufferLocation);
	this->mTreeModel.drawInstanced(aUniform, aBoneMatrices, this->mTreeCount, aTransformUniform, aNormalUniform);

	//lampposts
	this->mLightMatrices.bind(aInstanceBufferLocation);
	this->mLightModel.drawInstanced(aUniform, aBoneMatrices, this->mLightCount, aTransformUniform, aNormalUniform);

	//buildings
	for(uint8_t i = 0; i < 4; i++) {
		this->mBuildingMatrices[i].bind(aInstanceBufferLocation);
		this->mBuildingModels[i].drawInstanced(aUniform, aBoneMatrices, this->mBuildingCounts[i], aTransformUniform, aNormalUniform);
	}

	aBoolStateUniform.set(0);

	//track
	//disable culling - our generator doesnt care about winding order
	glDisable(GL_CULL_FACE);

	aUniform.update(&Map::msTrackMaterial->material, 0, 0);
	aUniform.set();

	this->mTrackArray.bind();

	//our bind slot is 1
	Map::msTrackMaterial->texture.bind(1);

	this->mTrackVertices.bind();
	this->mTrackIndices.bind();
	//this->mTrackVertices.drawPoints();
	this->mTrackIndices.draw(); //draw track last

	glEnable(GL_CULL_FACE);

	//signs
	if(this->mSigns.size() > 0) {
		this->mSignArray.bind();
		GlobalMaterialStore::copyDataToUniform(aUniform, this->mFirstSignMaterial, this->mLastSignMaterial);
		this->mSignVertices.bind();
		this->mSignIndices.bind();
		//this->mSignVertices.drawPoints();
		this->mSignIndices.draw();
	}

	//landmarks - should be only of each type per map
	for(Model& m : this->mLandmarkModels) {
		m.draw(aUniform, aBoneMatrices, aTransformUniform, aNormalUniform);
	}
}

uint64_t Map::getNextTrack(const uint64_t aCurrentId, const std::pair<uint8_t, uint64_t>& aEndNode, LineData::SwitchDirection aDirection) noexcept {
	if(aEndNode.first == 's') {
		//endpoint is switch
		int32_t secondNewTrackNode = 0;

		switch(aDirection) {
			case(LineData::SwitchDirection::STRAIGHT):
				secondNewTrackNode = this->mSwitches[aEndNode.second].front;
				break;
			case(LineData::SwitchDirection::LEFT):
				secondNewTrackNode = this->mSwitches[aEndNode.second].left;
				break;
			case(LineData::SwitchDirection::RIGHT):
				secondNewTrackNode = this->mSwitches[aEndNode.second].right;
				break;
			case(LineData::SwitchDirection::NO_SET):
				//we go against switch
				secondNewTrackNode = this->mSwitches[aEndNode.second].before;
				break;
			default:
				std::cerr << LogLevel::WARNING << "Unset switch!\n" << LogLevel::RESET;
				return UINT64_MAX;
		}

		//find track by 2 points - if our switch node actually switch and other is not (2-switch tracks not allowed)
		//non-switch tracks handled in other branch
		for(uint64_t i = 0; i < this->mTracks.size(); i++) {
			if(aCurrentId == i) continue;
			if(
				((this->mTracks[i].flags & TRACK_FLAG_SECOND_SWITCH) && this->mTracks[i].first == secondNewTrackNode && this->mTracks[i].second == (int32_t)aEndNode.second) ||
				((this->mTracks[i].flags & TRACK_FLAG_FIRST_SWITCH) && this->mTracks[i].second == secondNewTrackNode && this->mTracks[i].first == (int32_t)aEndNode.second)
			) { return i; }
		}
	}
	else {
		for(uint64_t i = 0; i < this->mTracks.size(); i++) {
			if(aCurrentId == i) continue;
			//translation: find track with same point which isnt ours and is not switch
			if(
				(this->mTracks[i].first == (int32_t)aEndNode.second && !((this->mTracks[i].flags & TRACK_FLAG_FIRST_SWITCH) > 0)) ||
				(this->mTracks[i].second == (int32_t)aEndNode.second && !((this->mTracks[i].flags & TRACK_FLAG_SECOND_SWITCH) > 0))
			) {
				return i;
			}
		}
	}

	return UINT64_MAX;
}

Track* Map::getStationByCode(std::string_view aCode) noexcept {
	for(Track& t : this->mTracks) {
		if(t.code == *(uint32_t*)aCode.data()) {
			return &t;
		}
	}
	return nullptr;
}
Node& Map::getNodeById(const uint64_t aId) noexcept {
	return this->mNodes[aId];
}
Switch& Map::getSwitchById(const uint64_t aId) noexcept {
	return this->mSwitches[aId];
}
Track& Map::getTrackById(const uint64_t aId) noexcept {
	return this->mTracks[aId];
}
bool Map::isTrackStation(const uint64_t aId) noexcept {
	return this->mTracks[aId].code != 0;
}

std::pair<uint8_t, uint64_t> Map::getOtherTrackPoint(const uint64_t aTrackId, const std::pair<uint8_t, uint64_t>& aEndNode) noexcept {
	auto& track = this->mTracks[aTrackId];
	auto& oldType = aEndNode.first;
	auto& oldId = aEndNode.second;

	bool isFirstSwitch = track.flags & TRACK_FLAG_FIRST_SWITCH;
	bool isSecondSwitch = track.flags & TRACK_FLAG_SECOND_SWITCH;
	bool isEndNodeFirst = ((int32_t)oldId == track.first) && ((isFirstSwitch && oldType == 's') || (!isFirstSwitch && oldType == 'n'));
	int32_t newId = isEndNodeFirst ? track.second : track.first;
	uint8_t newType = 'W';

	//second node is switch or not?
	//if our node first and first node is switch - SWITCH
	//if our node second and second node is switch - SWITCH
	//otherwise no
	if(isFirstSwitch && track.first == newId && !isEndNodeFirst) {
		newType = 's';
	}
	else if(isSecondSwitch && track.second == newId && isEndNodeFirst) {
		newType = 's';
	}
	else {
		newType = 'n';
	}

	return std::make_pair(newType, newId);
}

Map::~Map() noexcept {}
