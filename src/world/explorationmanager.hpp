#pragma once
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "worldstd.hpp"
#include "utilities/glmhash.hpp"

const std::string regionDir = "worlddata";
const std::string dataExt = ".exp";

const std::string pathSep =
#ifdef _WIN32
    "\\";
#else
    "/";
#endif

class ExplorationManager
{
	public:
		ExplorationManager();
		bool getChunkExplored(const ChunkCoord& coordinate);
		void setChunkExplored(const ChunkCoord& coordinate);
		void saveExploration();
	private:
		std::unordered_map<RegionCoord, std::vector<bool>> mExploreData;
		std::unordered_set<RegionCoord> mExploreChange;
		std::string mWorldName;
		
		std::string getFilename(RegionCoord regionLoc);
		void setWorldName(const std::string& name);
};
