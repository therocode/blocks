#include "world.h"
#include "localchunkprovider.h"

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
