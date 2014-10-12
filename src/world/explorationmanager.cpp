#include <fea/assert.hpp>
#include "explorationmanager.hpp"
#include <iostream>
#include <fstream>

ExplorationManager::ExplorationManager()
{
}

bool ExplorationManager::getChunkExplored(const ChunkCoord& coordinate)
{
	ExplorationRegionCoord regcoord = ChunkToExplorationRegion::convert(coordinate);
	ExplorationRegionChunkCoord regchunkcoord = ChunkToExplorationRegionChunk::convert(coordinate);

	if(mExploreData.count(regcoord) == 0)
	{
		std::cout << getFilename(regcoord) + dataExt << std::endl;
		std::ifstream expFile(getFilename(regcoord) + dataExt, std::ios::in | std::ios::binary);
		if(expFile.is_open())
		{
			std::vector<uint8_t> byteHolder(4096);
			expFile.read((char*)byteHolder.data(), 4096);
			expFile.close();
			
			std::vector<bool> explore(32*32*32);
			for(int x=0; x<4096; ++x)
			{
				explore[x*8  ] = byteHolder[x] >> 7;
				explore[x*8+1] = byteHolder[x] >> 6 & 1;
				explore[x*8+2] = byteHolder[x] >> 5 & 1;
				explore[x*8+3] = byteHolder[x] >> 4 & 1;
				explore[x*8+4] = byteHolder[x] >> 3 & 1;
				explore[x*8+5] = byteHolder[x] >> 2 & 1;
				explore[x*8+6] = byteHolder[x] >> 1 & 1;
				explore[x*8+7] = byteHolder[x]      & 1;
			}
			mExploreData.emplace(regcoord, explore);
		}
		else
		{
			return false;
		}
	}

	uint16_t pos = regchunkcoord.x + regchunkcoord.y * explorationRegionWidthInChunks * explorationRegionWidthInChunks + regchunkcoord.z * explorationRegionWidthInChunks;
	
	return mExploreData.at(regcoord)[pos];
}

void ExplorationManager::setChunkExplored(const ChunkCoord& coordinate)
{
	ExplorationRegionCoord regcoord = ChunkToExplorationRegion::convert(coordinate);
	ExplorationRegionChunkCoord regchunkcoord = ChunkToExplorationRegionChunk::convert(coordinate);

	if(mExploreData.count(regcoord) == 0)
	{
		std::vector<bool> explore(32*32*32);
		std::fill(explore.begin(), explore.end(), false);
		mExploreData.emplace(regcoord, explore);
	}
	
	uint16_t pos = regchunkcoord.x + regchunkcoord.y * explorationRegionWidthInChunks * explorationRegionWidthInChunks + regchunkcoord.z * explorationRegionWidthInChunks;
	
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
		
		std::ofstream expFile(getFilename(v) + dataExt, std::ios::out | std::ios::binary);
		expFile.write((char*)explorePrinter.data(), 4096);
		expFile.close();
	}
}

std::string ExplorationManager::getFilename(ExplorationRegionCoord regionLoc)
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
	FEA_ASSERT(name != "", "hej");
    mWorldName = name; //set name
}
