#include "world.hpp"
#include <iostream>
#include <fea/assert.hpp>
#include "worldmessages.hpp"

World::World(fea::MessageBus& b) :
    mBus(b)
{
}

ChunkReferenceMap World::getChunkMap() const
{
    return ChunkReferenceMap();
}

bool World::hasRegion(const RegionCoord& coordinate) const
{
    return mRegions.find(coordinate) != mRegions.end();
}

const Region& World::getRegion(const RegionCoord& coordinate) const
{
    return mRegions.at(coordinate);
}

void World::addRegion(const RegionCoord& coordinate, const Region& region)
{
    mRegions.emplace(coordinate, std::move(region));
}

void World::addChunk(const ChunkCoord& coordinate, const Chunk& chunk)
{
    RegionCoord region = chunkToRegion(coordinate);

    FEA_ASSERT(mRegions.count(region) != 0, "Trying to add chunk " + glm::to_string((glm::ivec3)coordinate) + " to region " + glm::to_string((glm::ivec2)region) + " but that region doesn't exist!");
    mRegions.at(region).addChunk(chunkToRegionChunk(coordinate), chunk);
}

void World::removeChunk(const ChunkCoord& coordinate)
{
    RegionCoord regionCoord = chunkToRegion(coordinate);

    FEA_ASSERT(mRegions.count(regionCoord) != 0, "Removing chunk " + glm::to_string((glm::ivec3)coordinate) + " from the world in region " + glm::to_string((glm::ivec2)regionCoord) + " but that region has not been added");

    Region& region = mRegions.at(regionCoord);

    FEA_ASSERT(region.hasChunk(chunkToRegionChunk(coordinate)), "Removing chunk " + glm::to_string((glm::ivec3)coordinate) + " from the world in region " + glm::to_string((glm::ivec2)regionCoord) + " but that chunk has not been added");

    region.removeChunk(chunkToRegionChunk(coordinate));

    if(region.getLoadedChunkAmount() == 0)
    {
        //std::cout << " this also removed a region\n";
        mRegions.erase(regionCoord);
        mBus.send(RegionDeletedMessage{regionCoord});
    }
}

void World::removeRegion(const RegionCoord& coordinate)
{
    FEA_ASSERT(mRegions.size() == 0, "This region which is about to be removed is actually not empty which is bad");
    mRegions.erase(coordinate);
}

VoxelType World::getVoxelType(const VoxelCoord& voxelCoordinate) const
{
    RegionCoord regionCoord = voxelToRegion(voxelCoordinate);

    if(hasRegion(regionCoord))
    {
        const Region& region = mRegions.at(regionCoord);
        RegionChunkCoord chunk = voxelToRegionChunk(voxelCoordinate);
        if(region.hasChunk(chunk))
            return region.getChunk(chunk).getVoxelType(voxelToChunkVoxel(voxelCoordinate));
        else
            return -1;
    }
    else
        return -1;
}

bool World::setVoxelType(const VoxelCoord& voxelCoordinate, VoxelType type)
{
    RegionCoord regionCoord = voxelToRegion(voxelCoordinate);

    if(hasRegion(regionCoord))
    {
        Region& region = mRegions.at(regionCoord);
        RegionChunkCoord chunk = voxelToRegionChunk(voxelCoordinate);
        if(region.hasChunk(chunk))
        {
            region.getChunk(chunk).setVoxelType(voxelToChunkVoxel(voxelCoordinate), type);
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void World::activateChunk(const ChunkCoord& coordinate)
{
    RegionCoord regionCoord = chunkToRegion(coordinate);

    if(mActiveRegions.count(regionCoord) == 0)
    {
        mActiveRegions.emplace(regionCoord, std::unordered_set<ChunkCoord>{coordinate});
    }
    else
    {
        mActiveRegions.at(regionCoord).insert(coordinate);
    }
}

void World::deactivateChunk(const ChunkCoord& coordinate)
{
    //std::cout << "want to delete chunk " << glm::to_string((glm::ivec3)coordinate) << "\n";
    RegionCoord regionCoord = chunkToRegion(coordinate);

    FEA_ASSERT(mActiveRegions.count(regionCoord) != 0, "Dehighlighting a region " + glm::to_string((glm::ivec2)regionCoord) + " which has not been highlighted. bad!");

    auto& highlightRegion = mActiveRegions.at(regionCoord);

    FEA_ASSERT(highlightRegion.count(coordinate) != 0, "Dehighlighting chunk " + glm::to_string((glm::ivec3)coordinate) + " which does not exist. bad!");

    mActiveRegions.at(regionCoord).erase(coordinate);

    bool regionCancelled = false;

    if(mActiveRegions.at(regionCoord).size() == 0)
    {
        mActiveRegions.erase(regionCoord);
        regionCancelled = true;
    }

    //if a chunk is deactivated and there is no region for that chunk, it must mean that the region is about to be generated in the other thread but hasn't been finished yet. The thread must be notified that the region and chunk is not needed anymore. But only if it is not needed by another chunk
    if(mRegions.count(regionCoord) == 0)
    {
        mBus.send(HaltChunkAndRegionGenerationMessage{coordinate, regionCancelled ? &regionCoord : nullptr});
        return;
    }

    auto& region = mRegions.at(regionCoord);

    //if a chunk is deactivated and doesn't exist, it has to be under generation in the other thread. notify the thread to stop generation
    if(region.hasChunk(chunkToRegionChunk(coordinate)) == 0)
    {
        mBus.send(HaltChunkAndRegionGenerationMessage{coordinate, nullptr});
        return;
    }

    removeChunk(coordinate);

    mBus.send(ChunkDeletedMessage{coordinate});
}
