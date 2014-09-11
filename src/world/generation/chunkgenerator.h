#pragma once
#include "../chunk.h"
#include "../region.h"

class ChunkGenerator
{
    public:
        Chunk generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const;
};
