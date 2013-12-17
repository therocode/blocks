#include "world.h"

ChunkReferenceMap World::getChunkMap() const
{
    return ChunkReferenceMap();
}

bool World::hasRegion(const RegionCoordinate& coordinate)
{
    return mRegions.find(coordinate) != mRegions.end();
}

const Region& World::getRegion(const RegionCoordinate& coordinate)
{
    return mRegions.at(coordinate);
}

void World::addRegion(const RegionCoordinate& coordinate, const Region& region)
{
    mRegions.emplace(coordinate, std::move(region));
}

void World::addChunk(const ChunkCoordinate& coordinate, const Chunk& chunk)
{
    RegionCoordinate region = chunkToRegion(coordinate);

    mRegions.at(region).addChunk(chunkToChunkRegion(coordinate), chunk);
}

VoxelType World::getVoxelType(const VoxelWorldCoordinate& voxelWorldCoordinate)
{
    RegionCoordinate region = chunkToRegion(voxelToChunk(voxelWorldCoordinate));
}
