#pragma once
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "worlddefines.hpp"
#include "../utilities/glmhash.hpp"

const uint32_t explorationRegionWidthInChunks = 32;

using ExplorationRegionCoord = glm::i64vec3;
using ExplorationRegionChunkCoord = glm::u8vec3;

using ChunkToExplorationRegion = CoordinateCoarseConvert<ChunkCoord, ExplorationRegionCoord, explorationRegionWidthInChunks>;
using ChunkToExplorationRegionChunk = CoordinateWrapConvert<ChunkCoord, ExplorationRegionChunkCoord, 0, explorationRegionWidthInChunks - 1>;

const std::string dataExt = ".exp";

class ExplorationManager
{
	public:
        ExplorationManager(const std::string& worldPath);
		bool getChunkExplored(const ChunkCoord& coordinate);
		void setChunkExplored(const ChunkCoord& coordinate);
		void saveExploration();
	private:
		std::unordered_map<ExplorationRegionCoord, std::vector<bool>> mExploreData;
		std::unordered_set<ExplorationRegionCoord> mExploreChange;
		std::string mWorldPath;
		
		std::string getFilename(ExplorationRegionCoord regionLoc);
};
