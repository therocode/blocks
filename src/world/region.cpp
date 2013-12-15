#include "region.h"
#include "worldmessages.h"
#include <iostream>

Region::Region(fea::MessageBus& bus) : mBus(bus)
{
}


bool Region::chunkIsLoaded(const ChunkRegionCoordinate& location) const
{
    return chunks.find(location) != chunks.end();
}

const Chunk& Region::getChunk(const ChunkRegionCoordinate& location) const
{
    return chunks.at(location);
}

Chunk& Region::getChunk(const ChunkRegionCoordinate& location)
{
    return chunks.at(location);
}

ChunkReferenceMap Region::getChunkList() const
{
    return ChunkReferenceMap();
}
