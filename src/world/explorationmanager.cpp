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
	mExploreChange.insert(regcoord);
}

void ExplorationManager::saveExploration()
{
	for(const auto &v : mExploreChange)
	{
		std::vector<uint8_t> explorePrinter(4096);
		for(int x=0; x<4096; ++x)
		{
			explorePrinter[x] = (mExploreData.at(v)[x*8  ] << 7) | (mExploreData.at(v)[x*8+1] << 6) |
								(mExploreData.at(v)[x*8+2] << 5) | (mExploreData.at(v)[x*8+3] << 4) |
								(mExploreData.at(v)[x*8+4] << 3) | (mExploreData.at(v)[x*8+5] << 2) |
								(mExploreData.at(v)[x*8+6] << 1) |  mExploreData.at(v)[x*8+7];
		}
		
		std::ofstream asd(getFilename(v) + dataExt, std::ios::out | std::ios::binary);
		asd.write((char*)explorePrinter.data(),4096);
		asd.close();
	}
}

std::string ExplorationManager::getFilename(RegionCoord regionLoc)
{
    std::string xPart = std::to_string(regionLoc.x);
    std::string yPart = std::to_string(regionLoc.y);

    if(xPart[0] == '-')
        xPart[0] = '_';
    if(yPart[0] == '-')
        yPart[0] = '_';

    return regionDir + pathSep + mWorldName + pathSep + xPart + "_" + yPart;  //NOTE: not sure why this needs to be casted... not good.
}

void ExplorationManager::setWorldName(const std::string& name)
{
    mWorldName = name;
}
