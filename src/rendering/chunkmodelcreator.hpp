#pragma once
#include "../utilities/glm.hpp"
#include "chunkmodel.hpp"
#include "../world/worlddefines.hpp"

class Chunk;

struct ChunkModelDelivery
{
    ChunkCoord coordinate;
    ChunkModel model;
};

class ChunkModelCreator
{
    public:
        ChunkModelDelivery generateChunkModel(const ChunkCoord& mainChunkCoord, const Chunk& mainChunk, const Chunk* topChunk, const Chunk* bottomChunk, const Chunk* frontChunk, const Chunk* backChunk, const Chunk* leftChunk, const Chunk* rightChunk);
};
