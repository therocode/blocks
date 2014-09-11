#pragma once
#include "../chunk.hpp"
#include "../region.hpp"

class ChunkGenerator
{
    public:
        Chunk generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const;
};
