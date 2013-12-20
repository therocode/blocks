#pragma once
#include "../chunk.h"

class Region;

class WorldGenerator
{
    public:
        Chunk generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const;
};
