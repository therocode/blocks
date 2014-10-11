#pragma once
#include <array>
#include <vector>
#include <unordered_map>
#include "worldstd.hpp"
#include "utilities/glmhash.hpp"

class ExplorationManager
{
	public:
		ExplorationManager();
		bool getChunkExplored(const ChunkCoord& coordinate);
		void setChunkExplored(const ChunkCoord& coordinate);
		void saveExploration(const ChunkCoord& coordinate);
	private:
		std::unordered_map<RegionCoord, std::vector<bool>> mExploreData;
};
