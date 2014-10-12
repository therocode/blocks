#pragma once
#include <fea/util.hpp>
#include "../../utilities/glm.hpp"
#include "../chunk.hpp"
#include "../biomedefines.hpp"
#include "../biome.hpp"


class ChunkGenerator
{
    public:
        Chunk generateChunk(const ChunkCoord& chunkCoordinate, const BiomeGrid& biomeData, const std::unordered_map<BiomeIndex, Biome>& biomes) const;
    private:
};
