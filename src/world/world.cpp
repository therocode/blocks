#include "world.hpp"
#include <iostream>

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
    std::cout << "got a chunk " << glm::to_string((glm::ivec3)coordinate) << " in the world\n";
    RegionCoord region = chunkToRegion(coordinate);

    mRegions.at(region).addChunk(chunkToRegionChunk(coordinate), chunk);
}

bool World::removeChunk(const ChunkCoord& coordinate)
{
    std::cout << "removing a chunk " << glm::to_string((glm::ivec3)coordinate) << " from the world";
    RegionCoord regionCoord = chunkToRegion(coordinate);

    Region& region = mRegions.at(regionCoord);
    region.removeChunk(chunkToRegionChunk(coordinate));

    if(region.getLoadedChunkAmount() == 0)
    {
        std::cout << " this also removed a region\n";
        mRegions.erase(regionCoord);
        return true;
    }
    std::cout << "\n";
    return false;
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
