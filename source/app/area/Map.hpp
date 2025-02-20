#ifndef EUROTRAM_AREA_MAP
#define EUROTRAM_AREA_MAP
#include "../objects/Objects.hpp"
#include "Line.hpp"

constexpr float TRACK_GAUGE = 1.435; //standard gauge
constexpr float TRACK_WIDTH = 0.30; //30cm width of track

constexpr uint64_t MAP_FILE_FORMAT_VERSION = 0;

//prefixes and extensions for loading texture files
constexpr std::string TEXPARCEL_TEXTURE_PREFIX = "texparcel/tp-";
constexpr std::string WALL_TEXTURE_PREFIX = "wall/wall-";
constexpr std::string TEXTURE_EXTENSION = "png";

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

	void draw(UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices, UniformMat4& aTransformUniform, UniformMat3& aNormalUniform, const uint64_t aInstanceBufferLocation, UniformInt& aBoolStateUniform) noexcept; //draw map FIRST

	uint64_t getNextTrack(const uint64_t aCurrentId, const uint64_t aEndNode, LineData::SwitchDirection aDirection) noexcept;
	Track* getStationByCode(std::string_view aCode) noexcept;

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
	ShaderBuffer mSwitchSignalIds;
	uint64_t mSwitchSignalCount;

	Model mSignalModel;
	//signal contains presignal id
	std::vector<Signal> mSignals;
	ShaderBuffer mSignalMatrices;
	ShaderBuffer mSignalIds;
	uint64_t mSignalCount;

	//also invisible
	std::vector<Radiobox> mRadioboxes;

	Model mStationPillarModel;
	std::vector<StationPillar> mPillars;
	ShaderBuffer mPillarMatrices;
	ShaderBuffer mPillarIds;
	uint64_t mPillarCount;

	Model mTreeModel;
	std::vector<Tree> mTrees;
	ShaderBuffer mTreeMatrices;
	ShaderBuffer mTreeIds;
	uint64_t mTreeCount;

	Model mLightModel;
	std::vector<Lightpole> mLights;
	ShaderBuffer mLightMatrices;
	ShaderBuffer mLightIds;
	uint64_t mLightCount;

	std::vector<Building> mBuildings;
	std::vector<Model> mBuildingModels; //mapped to type enum
	std::vector<ShaderBuffer> mBuildingMatrices; //mapped too
	uint64_t mBuildingCount;

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
