#ifndef EUROTRAM_AREA_MAP
#define EUROTRAM_AREA_MAP
#include "../objects/Objects.hpp"

constexpr uint64_t MAP_FILE_FORMAT_VERSION = 0;

void readLocationToString(std::ifstream& aStream, ObjectLocation& aLocation, const uint8_t aUnitsPerMeter) noexcept;
void readLocationToString(std::ifstream& aStream, RotatedObjectLocation& aLocation, const uint8_t aUnitsPerMeter) noexcept;

class Map {
public:
	Map() noexcept;
	Map(const std::string_view aFilename) noexcept;

	void open(const std::string_view aFilename) noexcept;

	//every object type is a) instanced and b) drawn only if in station code
	//we save instance matrices for objects in separate vectors for each object type
	//another vector - ids of instance matrices, we generate it according to this array
	//matrices passed in SSBOs, ids as well (in separate) - calculated ONCE
	//our goal: 1 draw call per visible object type

	void regenerateInstanceArray(StationCode aPrev, StationCode aCurrent, StationCode aNext, StationCode aAfterNext) noexcept; //only call on station code change

	void draw() noexcept; //draw map FIRST

	Track* getStationByCode(std::string_view aCode) noexcept;

	~Map() noexcept;
private:
	std::string mMapName;
	std::string mAuthorName;

	//invisible points
	std::vector<Node> mNodes;
	std::vector<Switch> mSwitches;

	//tracks are generated from vertices and textures on the fly - single draw call
	//here we need to keep vectors - id references from other parts, trigger boxes
	//track contains station code
	std::vector<Track> mTracks;
	VertexBuffer mTrackVertices;
	IndexBuffer mTrackIndices;

	Model mSwitchSignalModel;
	std::vector<SwitchSignal> mSwitchSignals;

	Model mSignalModel;
	//signal contains presignal id
	std::vector<Signal> mSignals;

	std::vector<Radiobox> mRadioboxes;

	Model mStationPillarModel;
	std::vector<StationPillar> mPillars;
	ShaderBuffer mPillarMatrices;
	ShaderBuffer mPillarIds;

	Model mTreeModel;
	std::vector<Tree> mTrees;
	ShaderBuffer mTreeMatrices;
	ShaderBuffer mTreeIds;

	Model mLightModel;
	std::vector<Lightpole> mLights;
	ShaderBuffer mLightMatrices;
	ShaderBuffer mLightIds;

	std::vector<Model> mBuildingModels;
	std::vector<Building> mBuildings;
	ShaderBuffer mBuildingMatrices;
	ShaderBuffer mBuildingIds;

	//landmarks always drawn
	std::vector<Landmark> mLandmarks;

	std::vector<Wall> mWalls;
	ShaderBuffer mWallMatrices;
	ShaderBuffer mWallIds;

	std::vector<Sign> mSigns;
	ShaderBuffer mSignMatrices;
	ShaderBuffer mSignIds;

	//texparcels are also generated from vertices and textures on the fly - single draw call
	VertexBuffer mTexparcelVertices;
	IndexBuffer mTexparcelIndices;
};

namespace UI {
	void drawMapInfoDebugWindow(Map& aMap) noexcept;
}

#endif
