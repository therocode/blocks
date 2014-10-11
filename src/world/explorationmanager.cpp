#include "explorationmanager.hpp"
#include <iostream>
#include <fstream>

ExplorationManager::ExplorationManager()
{
}

bool ExplorationManager::getChunkExplored(const ChunkCoord& coordinate)
{
	RegionCoord regcoord = chunkToRegion(coordinate);
	RegionChunkCoord regchunkcoord = chunkToRegionChunk(coordinate);

	if(mExploreData.count(regcoord) == 0)
	{
		return false;
	}

	uint16_t pos = regchunkcoord.x + regchunkcoord.y * regionChunkWidth * regionChunkWidth + regchunkcoord.z * regionChunkWidth;
	
	return mExploreData.at(regcoord)[pos];
}

void ExplorationManager::setChunkExplored(const ChunkCoord& coordinate)
{
	RegionCoord regcoord = chunkToRegion(coordinate);
	RegionChunkCoord regchunkcoord = chunkToRegionChunk(coordinate);

	if(mExploreData.count(regcoord) == 0)
	{
		std::vector<bool> explore(32*32*32);
		std::fill(explore.begin(), explore.end(), false);
		mExploreData.emplace(regcoord, explore);
	}
	
	uint16_t pos = regchunkcoord.x + regchunkcoord.y * regionChunkWidth * regionChunkWidth + regchunkcoord.z * regionChunkWidth;
	
	mExploreData.at(regcoord)[pos] = true;
}

void ExplorationManager::saveExploration(const ChunkCoord& coordinate) //worldname
{
	// RegionCoord regcoord = chunkToRegion(coordinate);
	// std::ofstream asd("zzz", std::ios::out | std::ios::binary);
	// asd.write(mExploreData.at(regcoord).data(),4096);
	// asd.close();
}
