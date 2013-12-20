#include "world.h"

ChunkReferenceMap World::getChunkMap() const
{
    return ChunkReferenceMap();
}

bool World::hasRegion(const RegionCoord& coordinate)
{
    return mRegions.find(coordinate) != mRegions.end();
}

const Region& World::getRegion(const RegionCoord& coordinate)
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

VoxelType World::getVoxelType(const VoxelCoord& voxelCoordinate)
{
    RegionCoord regionCoord = voxelToRegion(voxelCoordinate);

    if(hasRegion(regionCoord))
    {
        Region& region = mRegions.at(regionCoord);
        RegionChunkCoord chunk = voxelToRegionChunk(voxelCoordinate);
        if(region.hasChunk(chunk))
            return region.getChunk(chunk).getVoxelType(voxelToChunkVoxel(voxelCoordinate));
        else
            return -1;
    }
    else
        return -1;
}
