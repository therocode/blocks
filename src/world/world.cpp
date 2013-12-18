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

VoxelType World::getVoxelType(const VoxelCoord& voxelCoordinate)
{
    RegionCoordinate region = chunkToRegion(voxelToChunk(voxelCoordinate));

    //mRegions.at(region).getChunk(chunkToChunkRegion(voxelToChunk(voxelWorldCoordinate))).getVoxelType(worldToChunkInt(voxelWorldCoordinate.x, voxelWorldCoordinate.y, voxelWorldCoordinate.z));
    return 0;
}
