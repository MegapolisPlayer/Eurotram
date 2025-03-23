#ifndef EUROTRAM_AREA_MAP
#define EUROTRAM_AREA_MAP
#include "../objects/Objects.hpp"
#include "Line.hpp"

constexpr float TRACK_GAUGE = 1.435; //standard gauge
constexpr float TRACK_WIDTH = 0.30; //30cm width of track
constexpr float TRACK_HEIGHT = 0.20; //30cm width of track

constexpr uint64_t MAP_FILE_FORMAT_VERSION = 0;

//prefixes and extensions for loading texture files
constexpr std::string TEXPARCEL_TEXTURE_PREFIX = "texparcel/tp-";
constexpr std::string WALL_TEXTURE_PREFIX = "wall/wall-";
constexpr std::string TEXTURE_EXTENSION = "jpg";

//small modifier
constexpr float TRACK_Z_FIGHT_AVOIDANCE = 0.01;

void readLocationToString(std::ifstream& aStream, ObjectLocation& aLocation, const uint8_t aUnitsPerMeter) noexcept;
void readLocationToString(std::ifstream& aStream, RotatedObjectLocation& aLocation, const uint8_t aUnitsPerMeter) noexcept;

class Map {
public:
	Map() noexcept;
	Map(const std::string_view aFilename) noexcept;

	void open(const std::string_view aFilename) noexcept;

	//randomize building colors so they look better
	void randomizeBuildingColors() noexcept;

	//every object type is a) instanced and b) drawn only if in station code
	//we save instance matrices for objects in separate vectors for each object type
	//another vector - ids of instance matrices, we generate it according to this array
	//matrices passed in SSBOs, ids as well (in separate) - calculated ONCE
	//our goal: 1 draw call per visible object type
	void regenerateInstanceArray(StationCode aPrev, StationCode aCurrent, StationCode aNext, StationCode aAfterNext) noexcept; //only call on station code change

	//updates textures of signals, switch signals, station pillars
	//only touches those which belong to the station codes passed as params
	void updateTextures(StationCode aPrev, StationCode aCurrent, StationCode aNext, StationCode aAfterNext) noexcept;

	void draw(
		UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices, const uint64_t aInstanceBufferLocation, UniformInt& aBoolStateUniform,
		UniformMat4* aTransformUniform = nullptr, UniformMat3* aNormalUniform = nullptr
	) noexcept; //draw map FIRST

	uint64_t getNextTrack(const uint64_t aCurrentId, const std::pair<uint8_t, uint64_t>& aEndNode, LineData::SwitchDirection aDirection) noexcept;

	Node& getNodeById(const uint64_t aId) noexcept;
	Switch& getSwitchById(const uint64_t aId) noexcept;

	Track& getTrackById(const uint64_t aId) noexcept;
	Track* getStationByCode(std::string_view aCode) noexcept;

	bool isTrackStation(const uint64_t aId) noexcept;

	std::pair<uint8_t, uint64_t> getOtherTrackPoint(const uint64_t aTrackId, const std::pair<uint8_t, uint64_t>& aEndNode) noexcept;

	~Map() noexcept;
private:
	std::string mMapName;
	std::string mAuthorName;

	//invisible points
	std::vector<Node> mNodes;
	std::vector<Switch> mSwitches;

	VertexArray mOverheadWireArray;
	VertexBuffer mOverheadWireVertices;
	IndexBuffer mOverheadWireIndices;

	//tracks are generated from vertices and textures on the fly - single draw call
	//here we need to keep vectors - id references from other parts, trigger boxes
	//track contains station code
	VertexArray mTrackArray;
	std::vector<Track> mTracks;
	VertexBuffer mTrackVertices;
	IndexBuffer mTrackIndices;
	static GMSEntry* msTrackMaterial; //single material entry

	Model mSwitchSignalModel;
	std::vector<SwitchSignal> mSwitchSignals;
	ShaderBuffer mSwitchSignalMatrices;
	uint64_t mSwitchSignalCount;

	Model mSignalModel;
	//signal contains presignal id
	std::vector<Signal> mSignals;
	ShaderBuffer mSignalMatrices;
	uint64_t mSignalCount;

	Model mPoleModel; //pole for them
	Model mPresignalModel; //presignal sign
	ShaderBuffer mPoleMatrices;
	ShaderBuffer mPresignalMatrices;
	uint64_t mPoleCount;
	uint64_t mPresignalCount;

	//also invisible
	std::vector<Radiobox> mRadioboxes;

	Model mStationPillarModel;
	std::vector<StationPillar> mPillars;
	ShaderBuffer mPillarMatrices;
	uint64_t mPillarCount;

	Model mTreeModel;
	std::vector<Tree> mTrees;
	ShaderBuffer mTreeMatrices;
	uint64_t mTreeCount;

	Model mLightModel;
	std::vector<Lightpole> mLights;
	ShaderBuffer mLightMatrices;
	uint64_t mLightCount;

	std::vector<Building> mBuildings;
	std::vector<Model> mBuildingModels; //mapped to type enum
	std::vector<ShaderBuffer> mBuildingMatrices; //mapped too
	std::vector<uint64_t> mBuildingCounts;

	//landmarks always drawn
	std::vector<Model> mLandmarkModels;
	std::vector<Landmark> mLandmarks;

	//walls generated
	VertexArray mWallArray;
	VertexBuffer mWallVertices;
	IndexBuffer mWallIndices;
	std::vector<GMSEntry*> mWallMaterials;
	uint64_t mFirstWallMaterial;
	uint64_t mLastWallMaterial;

	//signs also generated but also stored
	VertexArray mSignArray;
	std::vector<Sign> mSigns;
	VertexBuffer mSignVertices;
	IndexBuffer mSignIndices;
	uint64_t mFirstSignMaterial;
	uint64_t mLastSignMaterial;

	//texparcels are also generated from vertices and textures on the fly - single draw call
	//split per material
	VertexArray mTexparcelArray;
	VertexBuffer mTexparcelVertices;
	IndexBuffer mTexparcelIndices;
	std::vector<GMSEntry*> mTexparcelMaterials;
	uint64_t mFirstTPMaterial;
	uint64_t mLastTPMaterial;

	bool mLoadedAssets;
};

namespace UI {
	void drawMapInfoDebugWindow(Map& aMap) noexcept;
}

#endif
