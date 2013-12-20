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
    RegionCoord region = chunkToRegion(voxelToChunk(voxelCoordinate));

    //mRegions.at(region).getChunk(chunkToChunkRegion(voxelToChunk(voxelWorldCoordinate))).getVoxelType(worldToChunkInt(voxelWorldCoordinate.x, voxelWorldCoordinate.y, voxelWorldCoordinate.z));
    return 0;
}
