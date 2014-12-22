#pragma once
#include "../utilities/glm.hpp"
#include "chunkmodel.hpp"
#include "../world/worlddefines.hpp"

class Chunk;

class ChunkModelCreator
{
    public:
        ChunkModel generateChunkModel(const ChunkCoord& mainChunkCoord, Chunk* mainChunk, Chunk* topChunk, Chunk* bottomChunk, Chunk* frontChunk, Chunk* backChunk, Chunk* leftChunk, Chunk* rightChunk);
};
