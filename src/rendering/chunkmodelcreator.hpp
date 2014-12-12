#pragma once
#include "../utilities/glm.hpp"
#include "model.hpp"
#include "../world/worlddefines.hpp"

class Chunk;

class ChunkModelCreator
{
    public:
        Model generateChunkModel(const ChunkCoord& mainChunkCoord, Chunk* mainChunk, Chunk* topChunk, Chunk* bottomChunk, Chunk* frontChunk, Chunk* backChunk, Chunk* leftChunk, Chunk* rightChunk);
};
