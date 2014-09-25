#pragma once
#include <fea/util.hpp>
#include "../../blockstd.hpp"
#include "../chunk.hpp"

class RegionDataFragment;
class BiomeStorage;

class ChunkGenerator
{
    public:
        Chunk generateChunk(const ChunkCoord& chunkCoordinate, const RegionDataFragment& regionFragment, const BiomeStorage& biomes) const;
    private:
};
