#pragma once
#include "chunk.h"
#include <unordered_map>
#include <vector>

class World
{
    private:
        std::unordered_map<ChunkCoordinate, uint32_t> chunkIndices;
        std::vector<Chunk> chunks;
};
