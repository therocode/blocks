#pragma once
#include "../chunk.hpp"
#include "../region.hpp"
#include <fea/util.hpp>

class ChunkGenerator
{
    public:
        Chunk generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const;
    private:
        fea::Noise mNoise;
};
