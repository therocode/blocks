#pragma once
#include "../chunk.hpp"
#include "../region.hpp"
#include "biomestorage.hpp"
#include <fea/util.hpp>

class ChunkGenerator
{
    public:
        Chunk generateChunk(const ChunkCoord& chunkCoordinate, const Region& region, const BiomeStorage& biomes) const;
    private:
        fea::Noise mNoise;
};
