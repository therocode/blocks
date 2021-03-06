#include <fea/assert.hpp>
#include "explorationmanager.hpp"
#include <fstream>

ExplorationManager::ExplorationManager(const std::string& worldPath) :
    mWorldPath(worldPath)
{
}

bool ExplorationManager::getChunkExplored(const ChunkCoord& coordinate)
{
	ExplorationRegionCoord regcoord = ChunkToExplorationRegion::convert(coordinate);
	ExplorationRegionChunkCoord regchunkcoord = ChunkToExplorationRegionChunk::convert(coordinate);

	if(mExploreData.count(regcoord) == 0)
	{
		std::ifstream expFile(getFilename(regcoord) + explorationExt, std::ios::in | std::ios::binary);
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
	for(const auto &regionCoord : mExploreChange)
	{
		std::vector<uint8_t> exploreWriter(4096);
		for(int x=0; x<4096; ++x)
		{
			FEA_ASSERT(mExploreData.count(regionCoord) > 0, std::to_string(regionCoord.x) + " " + std::to_string(regionCoord.y) + " " + std::to_string(regionCoord.z) + "doesn't exist");
			
			std::vector<bool> &changedRegion = mExploreData.at(regionCoord);
			exploreWriter[x] = (changedRegion[x*8  ] << 7) | (changedRegion[x*8+1] << 6) |
							   (changedRegion[x*8+2] << 5) | (changedRegion[x*8+3] << 4) |
							   (changedRegion[x*8+4] << 3) | (changedRegion[x*8+5] << 2) |
							   (changedRegion[x*8+6] << 1) |  changedRegion[x*8+7];
		}
		
		std::ofstream expFile(getFilename(regionCoord) + explorationExt, std::ios::out | std::ios::binary);
		expFile.write((char*)exploreWriter.data(), 4096);
		expFile.close();
	}
	
	mExploreChange.clear();
}

void ExplorationManager::saveRegionExploration(const ChunkCoord& coordinate)
{
	ExplorationRegionCoord regionCoord = ChunkToExplorationRegion::convert(coordinate);
	
	if(mExploreChange.count(regionCoord))
	{
		std::vector<uint8_t> exploreWriter(4096);
		for(int x=0; x<4096; ++x)
		{
			FEA_ASSERT(mExploreData.count(regionCoord) > 0, std::to_string(regionCoord.x) + " " + std::to_string(regionCoord.y) + " " + std::to_string(regionCoord.z) + "doesn't exist");
			
			std::vector<bool> &changedRegion = mExploreData.at(regionCoord);
			exploreWriter[x] = (changedRegion[x*8  ] << 7) | (changedRegion[x*8+1] << 6) |
							   (changedRegion[x*8+2] << 5) | (changedRegion[x*8+3] << 4) |
							   (changedRegion[x*8+4] << 3) | (changedRegion[x*8+5] << 2) |
							   (changedRegion[x*8+6] << 1) |  changedRegion[x*8+7];
		}
		
		std::ofstream expFile(getFilename(regionCoord) + explorationExt, std::ios::out | std::ios::binary);
		expFile.write((char*)exploreWriter.data(), 4096);
		expFile.close();
	}
}

void ExplorationManager::activateChunk(const ChunkCoord& coordinate)
{
	mExplorationGrid.set(coordinate, true);
}

void ExplorationManager::deactivateChunk(const ChunkCoord& coordinate)
{
	auto activatedCell = mExplorationGrid.set(coordinate, false);
	if(!activatedCell.empty())
	{
		saveRegionExploration(coordinate);
		mExploreData.erase(coordinate);
		mExploreChange.erase(ChunkToExplorationRegion::convert(coordinate));
	}
}

std::string ExplorationManager::getFilename(ExplorationRegionCoord regionLoc)
{
    std::string xPart = std::to_string(regionLoc.x);
    std::string yPart = std::to_string(regionLoc.y);
    std::string zPart = std::to_string(regionLoc.z);

    if(xPart[0] == '-')
        xPart[0] = '_';
    if(yPart[0] == '-')
        yPart[0] = '_';
	if(zPart[0] == '-')
        zPart[0] = '_';

    return mWorldPath + "/" + xPart + "_" + yPart + "_" + zPart;
}
