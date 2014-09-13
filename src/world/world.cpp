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

    mRegions.at(region).addChunk(chunkToRegionChunk(coordinate), chunk);
}

bool World::removeChunk(const ChunkCoord& coordinate)
{
    RegionCoord regionCoord = chunkToRegion(coordinate);

    Region& region = mRegions.at(regionCoord);
    region.removeChunk(chunkToRegionChunk(coordinate));

    //if(region.getLoadedChunkAmount() == 0)
    //{
    //    std::cout << " this also removed a region\n";
    //    mRegions.erase(regionCoord);
    //    return true;
    //}
    return false;
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
    //when a chunk is deactivated, if it doesn't exist the thread hasn't finished generating it. in that case, the thread needs to be notified of instant termination of its generation and any such generated chunk has to be deleted. in this case, don't send RegionDeletedMessage since the ChunkAddedMessage won't be sent either. Implement this.
    std::cout << "want to delete chunk " << glm::to_string((glm::ivec3)coordinate) << "\n";
    RegionCoord regionCoord = chunkToRegion(coordinate);

    FEA_ASSERT(mActiveRegions.count(regionCoord) == 1, "If the region of the chunk to be removed does not exist, something is wrong");

    auto& region = mActiveRegions.at(regionCoord);

    FEA_ASSERT(region.count(coordinate) == 1, "The deactivated chunk has not been activated. this is bad");

    region.erase(coordinate);

    if(region.size() == 0)
    {
        mActiveRegions.erase(regionCoord);

        mBus.send(RegionDeletedMessage{regionCoord});
    }
}
