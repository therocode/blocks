#pragma once
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "worlddefines.hpp"
#include "utilities/glmhash.hpp"

const uint32_t explorationRegionWidthInChunks = 16;
const uint32_t explorationRegionWidth = explorationRegionWidthInChunks * chunkWidth;

using ExplorationRegionCoord = glm::i64vec3;
using ExplorationRegionChunkCoord = glm::u8vec3;

using ChunkToExplorationRegion = CoordinateCoarseConvert<ChunkCoord, ExplorationRegionCoord, explorationRegionWidth>;
using ChunkToExplorationRegionChunk = CoordinateWrapConvert<ChunkCoord, ExplorationRegionChunkCoord, 0, explorationRegionWidth - 1>;

const std::string regionDir = "worlddata";
const std::string dataExt = ".exp";

class ExplorationManager
{
	public:
		ExplorationManager();
		bool getChunkExplored(const ChunkCoord& coordinate);
		void setChunkExplored(const ChunkCoord& coordinate);
		void saveExploration();
		void setWorldName(const std::string& name);
	private:
		std::unordered_map<ExplorationRegionCoord, std::vector<bool>> mExploreData;
		std::unordered_set<ExplorationRegionCoord> mExploreChange;
		std::string mWorldName;
		
		std::string getFilename(ExplorationRegionCoord regionLoc);
};
