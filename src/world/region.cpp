#include "region.h"
#include <iostream>

bool Region::chunkIsLoaded(const RegionChunkCoord& location) const
{
    return chunks.find(location) != chunks.end();
}

const Chunk& Region::getChunk(const RegionChunkCoord& location) const
{
    return chunks.at(location);
}

Chunk& Region::getChunk(const RegionChunkCoord& location)
{
    return chunks.at(location);
}

ChunkReferenceMap Region::getChunkList() const
{
    return ChunkReferenceMap();
}

void Region::addChunk(const RegionChunkCoord& coordinate, const Chunk& chunk)
{
    chunks.emplace(coordinate, std::move(chunk));
}
