#include "region.h"
#include <iostream>

Region::Region()
{

}

Region::Region(const IntensityMap& heightmap)
{
    mHeightmap = heightmap;
}

Region::Region(IntensityMap&& heightmap)
{
    std::swap(mHeightmap, heightmap);
}

bool Region::hasChunk(const RegionChunkCoord& location) const
{
    return mChunks.find(location) != mChunks.end();
}

const Chunk& Region::getChunk(const RegionChunkCoord& location) const
{
    return mChunks.at(location);
}

Chunk& Region::getChunk(const RegionChunkCoord& location)
{
    return mChunks.at(location);
}

ChunkReferenceMap Region::getChunkList() const
{
    return ChunkReferenceMap();
}

void Region::addChunk(const RegionChunkCoord& coordinate, const Chunk& chunk)
{
    mChunks.emplace(coordinate, std::move(chunk));
}

const IntensityMap& Region::getHeightmap() const
{
    return mHeightmap;
}
