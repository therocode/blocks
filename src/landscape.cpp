#include "landscape.h"

void Landscape::setChunkProvider(ChunkProvider* del)
{
    chunkDeliverer = del;
}

Chunk& Landscape::loadChunk(const ChunkCoordinate& location)
{
    chunks.push_back(chunkDeliverer->fetchChunk(location));
    chunkIndices.emplace(location, chunks.size() - 1);

    return chunks[chunks.size() - 1];
}

bool Landscape::chunkIsLoaded(const ChunkCoordinate& location) const
{
    return chunkIndices.find(location) != chunkIndices.end();
}

const Chunk& Landscape::getChunk(const ChunkCoordinate& location) const
{
    return chunks[chunkIndices.at(location)];
}
