#include "landscape.h"

void Landscape::setChunkDeliverer(ChunkDeliverer* del)
{
    chunkDeliverer = del;
}

Chunk& Landscape::loadChunk(const ChunkCoordinate& location)
{
    chunks.push_back(chunkDeliverer->fetchChunk(location));
    chunkIndices.emplace(location, chunks.size() - 1);

    return chunks[chunks.size() - 1];
}

const Chunk& Landscape::getChunk(const ChunkCoordinate& location) const
{
    return chunks[chunkIndices.at(location)];
}
