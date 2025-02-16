#include "Map.hpp"

void readLocationToString(std::ifstream& aStream, ObjectLocation& aLocation, const uint8_t aUnitsPerMeter) noexcept {
	std::string buffer;
	readBytesToString(aStream, buffer, 3*sizeof(uint32_t));
	aLocation.x = float(((int32_t*)buffer.data())[0]) / aUnitsPerMeter;
	aLocation.y = float(((int32_t*)buffer.data())[1]) / aUnitsPerMeter;
	aLocation.h = float(((int32_t*)buffer.data())[2]) / aUnitsPerMeter;
}
void readLocationToString(std::ifstream& aStream, RotatedObjectLocation& aLocation, const uint8_t aUnitsPerMeter) noexcept {
	std::string buffer;
	readBytesToString(aStream, buffer, 4*sizeof(uint32_t));
	aLocation.x = float(((int32_t*)buffer.data())[0]) / aUnitsPerMeter;
	aLocation.y = float(((int32_t*)buffer.data())[1]) / aUnitsPerMeter;
	aLocation.h = float(((int32_t*)buffer.data())[2]) / aUnitsPerMeter;
	aLocation.r = float(((int32_t*)buffer.data())[3]) / aUnitsPerMeter;
}

GMSEntry* Map::msTrackMaterial = nullptr;

Map::Map() noexcept
	: mTrackVertices(nullptr, 0, STANDARD_MODEL_VERTEX_FLOAT_AMOUNT), mTrackIndices(nullptr, 0),
	mSwitchSignalModel("./SwitchSignal.glb"), mSignalModel("./Signal.glb"),
	mStationPillarModel("./Stop.glb"), mPillarMatrices(nullptr, 0), mPillarIds(nullptr, 0),
	mTreeModel("./Tree.glb"), mTreeMatrices(nullptr, 0), mTreeIds(nullptr, 0),
	mLightModel("./Streetlamp.glb"), mLightMatrices(nullptr, 0), mLightIds(nullptr, 0),
	mBuildingMatrices(nullptr, 0), mBuildingIds(nullptr, 0),
	mWallMatrices(nullptr, 0), mWallIds(nullptr, 0),
	mSignMatrices(nullptr, 0), mSignIds(nullptr, 0),
	mTexparcelVertices(nullptr, 0, STANDARD_MODEL_VERTEX_FLOAT_AMOUNT), mTexparcelIndices(nullptr, 0) {}

Map::Map(const std::string_view aFilename) noexcept
	: mTrackVertices(nullptr, 0, STANDARD_MODEL_VERTEX_FLOAT_AMOUNT), mTrackIndices(nullptr, 0),
	mSwitchSignalModel("./SwitchSignal.glb"), mSignalModel("./Signal.glb"),
	mStationPillarModel("./Stop.glb"), mPillarMatrices(nullptr, 0), mPillarIds(nullptr, 0),
	mTreeModel("./Tree.glb"), mTreeMatrices(nullptr, 0), mTreeIds(nullptr, 0),
	mLightModel("./Streetlamp.glb"), mLightMatrices(nullptr, 0), mLightIds(nullptr, 0),
	mBuildingMatrices(nullptr, 0), mBuildingIds(nullptr, 0),
	mWallMatrices(nullptr, 0), mWallIds(nullptr, 0),
	mSignMatrices(nullptr, 0), mSignIds(nullptr, 0),
	mTexparcelVertices(nullptr, 0, STANDARD_MODEL_VERTEX_FLOAT_AMOUNT), mTexparcelIndices(nullptr, 0)
	{
	this->open(aFilename);
}

void Map::open(const std::string_view aFilename) noexcept {
	//values for typical metal - only load once
	if(!Map::msTrackMaterial) {
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

	//print header info - amounts and author
	std::cout << "Nodes amount: " << nodeAmount << '\n';
	std::cout << "Switch amount: " << switchAmount << '\n';
	std::cout << "Track amount: " << trackAmount << '\n';
	std::cout << "Switch Signal amount: " << switchSignalAmount << '\n';
	std::cout << "Signal amount: " << signalAmount << '\n';
	std::cout << "Radiobox amount: " << radioboxAmount << '\n';
	std::cout << "Pillar amount: " << pillarAmount << '\n';
	std::cout << "Tree amount: " << treeAmount << '\n';
	std::cout << "Streetlamp amount: " << lampAmount << '\n';
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
			//no units per meter division here
			this->mTracks.back().heightpoints[i] = (*((int32_t*)buffer.data())) * 100.0;
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
		this->mSignals.back().poleHeight =  *((int32_t*)buffer.data());

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
		readBytesToString(fileHandle, buffer, 4);
		this->mLandmarks.back().landmarkCode = *(uint32_t*)buffer.data();
	}

	//Wall list
	this->mWalls.reserve(wallAmount);
	for(uint32_t i = 0; i < wallAmount; i++) {
		this->mWalls.push_back({});
		readLocationToString(fileHandle, this->mWalls.back().firstPoint, unitsPerMeter);
		readLocationToString(fileHandle, this->mWalls.back().secondPoint, unitsPerMeter);

		readBytesToString(fileHandle, buffer, 2);
		this->mWalls.back().height = *((int16_t*)buffer.data());

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
		this->mSigns.back().type = (SignType)*((int16_t*)buffer.data());
	}

	{
		//Texparcel list

		//generate texparcel vertices
		std::vector<Vertex> texparcelVertices;

		for(uint32_t i = 0; i < texparcelAmount; i++) {
			//texparcel has 4 vertices - convert from clockwise in file to CCW in GL
			readBytesToString(fileHandle, buffer, 48); //3 values * 4 bytes * 4 points - 12 values
			for(int8_t j = 3; j >= 0; j--) {
				texparcelVertices.push_back({});
				texparcelVertices.back().position.x = ((int32_t*)buffer.data())[j*3+0]; //x
				texparcelVertices.back().position.y = ((int32_t*)buffer.data())[j*3+2]; //h height
				texparcelVertices.back().position.z = ((int32_t*)buffer.data())[j*3+1]; //y
			}
			//already CCW order
			glm::vec3 normal = Math::normals(
				texparcelVertices[texparcelVertices.size()-4].position,
				texparcelVertices[texparcelVertices.size()-3].position,
				texparcelVertices[texparcelVertices.size()-2].position,
				texparcelVertices[texparcelVertices.size()-1].position
			);

			//normals (set same for all)
			for(int8_t j = 3; j >= 0; j--) {
				texparcelVertices[texparcelVertices.size()-i-1].normal = normal;
			}

			//tex coords - in CCW order from bottom left
			texparcelVertices[texparcelVertices.size()-4].texCoords = glm::vec2(0.0, 0.0);
			texparcelVertices[texparcelVertices.size()-3].texCoords = glm::vec2(0.0, 1.0);
			texparcelVertices[texparcelVertices.size()-2].texCoords = glm::vec2(1.0, 1.0);
			texparcelVertices[texparcelVertices.size()-1].texCoords = glm::vec2(0.0, 1.0);

			readBytesToString(fileHandle, buffer, 8); //2 station codes
			std::getline(fileHandle, buffer, '\0'); //material name

			//TODO assign material if doesnt yet exist, otherwise assign ID
			if(!GlobalMaterialStore::getByName(buffer)) {
				this->mTexparcelMaterials.push_back(GlobalMaterialStore::add());
				this->mTexparcelMaterials.back();
			};
		}

		//generate texparcel indices
		std::vector<GLuint> texparcelIndices;
		for(uint32_t i = 0; i < texparcelVertices.size(); i+=4) {
			texparcelIndices.insert(texparcelIndices.end(), {i, 1+i, 2+i, 2+i, 3+i, i});
		}

		this->mArray.bind();
		//set data
		this->mTexparcelVertices = VertexBuffer((float*)texparcelVertices.data(), texparcelVertices.size(), STANDARD_MODEL_VERTEX_FLOAT_AMOUNT);
		this->mTexparcelVertices.enableStandardAttributes(&this->mArray);
		this->mTexparcelIndices = IndexBuffer(texparcelIndices.data(), texparcelIndices.size());
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

			if(t.flags & TRACK_FLAG_BEZIER) {
				trackVertices.reserve(trackVertices.size() + Math::DEFAULT_BEZIER_PRECISION*4);
				trackIndices.reserve(trackIndices.size() + Math::DEFAULT_BEZIER_PRECISION/4*6); //for every 4 vertices 6 indices

				//join bezier from previous track
				//[1,3,4 ; 3,6,4]
				trackIndices.insert(trackIndices.end(), {
					(uint32_t)trackVertices.size()-4 + 1,
					(uint32_t)trackVertices.size()-4 + 3,
					(uint32_t)trackVertices.size()-4 + 4,
					(uint32_t)trackVertices.size()-4 + 3,
					(uint32_t)trackVertices.size()-4 + 6,
					(uint32_t)trackVertices.size()-4 + 4
				});

				std::vector<Math::BezierPoint> curve = Math::bezier(
					firstNode,
					glm::vec2(t.point1x, t.point1y),
					glm::vec2(t.point2x, t.point2y),
					secondNode
				);

				//TODO add height loading
				//TODO make 2 tracks instead of one

				std::vector<Math::BezierPoint> topCurve = Math::moveBezier(curve, glm::vec2(TRACK_GAUGE/2+TRACK_WIDTH/2));
				std::vector<Math::BezierPoint> topInnerCurve = Math::moveBezier(curve, glm::vec2(TRACK_GAUGE/2));

				std::vector<Math::BezierPoint> bottomCurve = Math::moveBezier(curve, glm::vec2(-TRACK_GAUGE/2-TRACK_WIDTH/2));
				std::vector<Math::BezierPoint> bottomInnerCurve = Math::moveBezier(curve, glm::vec2(-TRACK_GAUGE/2));

				//add in groups of 4 - quads
				//2 from each bezier curve
				//generate texcoords based on this

				for(uint32_t i = 0; i < topCurve.size()-1; i+=2) {
					//H values are Y axis

					//rail 1
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(topCurve[i].x, 0.0, topCurve[i].y);
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(topCurve[i+1].x, 0.0, topCurve[i+1].y);
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(topInnerCurve[i].x, 0.0, topInnerCurve[i].y);
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(topInnerCurve[i+1].x, 0.0, topInnerCurve[i+1].y);

					//rail 2
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(bottomCurve[i].x, 0.0, bottomCurve[i].y);
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(bottomCurve[i+1].x, 0.0, bottomCurve[i+1].y);
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(bottomInnerCurve[i].x, 0.0, bottomInnerCurve[i].y);
					trackVertices.push_back({});
					trackVertices.back().position = glm::vec3(bottomInnerCurve[i+1].x, 0.0, bottomInnerCurve[i+1].y);

					//index buffer
					//[0,2,1 ; 2,3,1] top
					//[4,6,5 ; 6,7,5] bottom
					trackIndices.insert(trackIndices.end(), {
						(uint32_t)trackVertices.size()-8 + 0,
						(uint32_t)trackVertices.size()-8 + 2,
						(uint32_t)trackVertices.size()-8 + 1,
						(uint32_t)trackVertices.size()-8 + 2,
						(uint32_t)trackVertices.size()-8 + 3,
						(uint32_t)trackVertices.size()-8 + 1,

						(uint32_t)trackVertices.size()-8 + 4,
						(uint32_t)trackVertices.size()-8 + 6,
						(uint32_t)trackVertices.size()-8 + 5,
						(uint32_t)trackVertices.size()-8 + 6,
						(uint32_t)trackVertices.size()-8 + 7,
						(uint32_t)trackVertices.size()-8 + 5,
					});
					//if not last
					if(i < topCurve.size()-3) {
						//connect to next box
						//[1,3,8 ; 3,10,8] top
						//[5,7,12; 7,14,12] bottom
						trackIndices.insert(trackIndices.end(), {
							(uint32_t)trackVertices.size()-8 + 1,
							(uint32_t)trackVertices.size()-8 + 3,
							(uint32_t)trackVertices.size()-8 + 8,
							(uint32_t)trackVertices.size()-8 + 3,
							(uint32_t)trackVertices.size()-8 + 10,
							(uint32_t)trackVertices.size()-8 + 8,

							(uint32_t)trackVertices.size()-8 + 5,
							(uint32_t)trackVertices.size()-8 + 7,
							(uint32_t)trackVertices.size()-8 + 12,
							(uint32_t)trackVertices.size()-8 + 7,
							(uint32_t)trackVertices.size()-8 + 14,
							(uint32_t)trackVertices.size()-8 + 12,
						});
					}

					trackVertices[(uint32_t)trackVertices.size()-8+0].texCoords = glm::vec2(float(i)/topCurve.size(), 1.0);
					trackVertices[(uint32_t)trackVertices.size()-8+1].texCoords = glm::vec2(float(i+1)/topCurve.size(), 1.0);
					trackVertices[(uint32_t)trackVertices.size()-8+2].texCoords = glm::vec2(float(i)/topCurve.size(), 0.0);
					trackVertices[(uint32_t)trackVertices.size()-8+3].texCoords = glm::vec2(float(i+1)/topCurve.size(), 0.0);
					trackVertices[(uint32_t)trackVertices.size()-8+4].texCoords = glm::vec2(float(i)/topCurve.size(), 1.0);
					trackVertices[(uint32_t)trackVertices.size()-8+5].texCoords = glm::vec2(float(i+1)/topCurve.size(), 1.0);
					trackVertices[(uint32_t)trackVertices.size()-8+6].texCoords = glm::vec2(float(i)/topCurve.size(), 0.0);
					trackVertices[(uint32_t)trackVertices.size()-8+7].texCoords = glm::vec2(float(i+1)/topCurve.size(), 0.0);
				}
			}
			else {
				//just linear points - same order

				glm::vec2 dirVector = Math::getPerpendicularVectorFromPoints(firstNode, secondNode);
				glm::vec2 gaugeVector = dirVector*(TRACK_GAUGE/2);
				glm::vec2 gaugeLongVector = dirVector*(TRACK_GAUGE/2+TRACK_WIDTH/2);

				//rail 1
				trackVertices.emplace_back(glm::vec3(firstNode.x+gaugeLongVector.x, 0.0, firstNode.y+gaugeLongVector.y));
				trackVertices.emplace_back(glm::vec3(secondNode.x+gaugeLongVector.x, 0.0, secondNode.y+gaugeLongVector.y));
				trackVertices.emplace_back(glm::vec3(firstNode.x+gaugeVector.x, 0.0, firstNode.y+gaugeVector.y));
				trackVertices.emplace_back(glm::vec3(secondNode.x+gaugeVector.x, 0.0, secondNode.y+gaugeVector.y));

				trackIndices.insert(trackIndices.end(), {
					(uint32_t)trackVertices.size()-4 + 0,
					(uint32_t)trackVertices.size()-4 + 2,
					(uint32_t)trackVertices.size()-4 + 1,
					(uint32_t)trackVertices.size()-4 + 2,
					(uint32_t)trackVertices.size()-4 + 3,
					(uint32_t)trackVertices.size()-4 + 1
				});

				//tex coords
				trackVertices[(uint32_t)trackVertices.size()-4].texCoords = glm::vec2(0.0, 1.0);
				trackVertices[(uint32_t)trackVertices.size()-3].texCoords = glm::vec2(1.0, 1.0);
				trackVertices[(uint32_t)trackVertices.size()-2].texCoords = glm::vec2(0.0, 0.0);
				trackVertices[(uint32_t)trackVertices.size()-1].texCoords = glm::vec2(1.0, 0.0);

				//rail 2
				trackVertices.emplace_back(glm::vec3(firstNode.x-gaugeVector.x, 0.0, firstNode.y-gaugeVector.y));
				trackVertices.emplace_back(glm::vec3(secondNode.x-gaugeVector.x, 0.0, secondNode.y-gaugeVector.y));
				trackVertices.emplace_back(glm::vec3(firstNode.x-gaugeLongVector.x, 0.0, firstNode.y-gaugeLongVector.y));
				trackVertices.emplace_back(glm::vec3(secondNode.x-gaugeLongVector.x, 0.0, secondNode.y-gaugeLongVector.y));

				trackIndices.insert(trackIndices.end(), {
					(uint32_t)trackVertices.size()-4 + 0,
					(uint32_t)trackVertices.size()-4 + 2,
					(uint32_t)trackVertices.size()-4 + 1,
					(uint32_t)trackVertices.size()-4 + 2,
					(uint32_t)trackVertices.size()-4 + 3,
					(uint32_t)trackVertices.size()-4 + 1
				});

				//tex coords
				trackVertices[(uint32_t)trackVertices.size()-4].texCoords = glm::vec2(0.0, 0.0);
				trackVertices[(uint32_t)trackVertices.size()-3].texCoords = glm::vec2(1.0, 0.0);
				trackVertices[(uint32_t)trackVertices.size()-2].texCoords = glm::vec2(0.0, 1.0);
				trackVertices[(uint32_t)trackVertices.size()-1].texCoords = glm::vec2(1.0, 1.0);
			}
		}

		for(uint32_t i = 0; i < trackVertices.size(); i+=4) {
			//1,0,2,3
			glm::vec3 normal = Math::normals(
				trackVertices[i+1].position,
				trackVertices[i+0].position,
				trackVertices[i+2].position,
				trackVertices[i+3].position
			);

			trackVertices[i+0].normal = normal;
			trackVertices[i+1].normal = normal;
			trackVertices[i+2].normal = normal;
			trackVertices[i+3].normal = normal;
		}

		this->mArray.bind();

		std::cout << "Map track vertices: " << trackVertices.size() << ", indices: " << trackIndices.size() << '\n';

		this->mTrackVertices = VertexBuffer((GLfloat*)trackVertices.data(), trackVertices.size(), STANDARD_MODEL_VERTEX_FLOAT_AMOUNT);
		this->mTrackVertices.enableStandardAttributes(&this->mArray);

		this->mTrackIndices = IndexBuffer((GLuint*)trackIndices.data(), trackIndices.size());
		this->mTrackIndices.bind();
	}
}

void Map::regenerateInstanceArray(StationCode aPrev, StationCode aCurrent, StationCode aNext, StationCode aAfterNext) noexcept {

}

void Map::draw(UniformMaterial& aUniform) noexcept {
	//this->mTexparcelIndices.draw();

	//this->mBuildingIds;

	this->mArray.bind();

	aUniform.update(&Map::msTrackMaterial->material);
	aUniform.set();

	//our bind slot is 1
	Map::msTrackMaterial->texture.bind(1);

	this->mTrackVertices.bind();
	this->mTrackIndices.bind();
	this->mTrackVertices.drawPoints();
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
