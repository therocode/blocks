#pragma once
#include "chunkmap.hpp"
#include "biomedefines.hpp"
#include "biome.hpp"
#include "field.hpp"
#include "ranges.hpp"

struct WorldData
{
    ChunkMap voxels;
    std::vector<BiomeIndex> biomes;
    BiomeStorage biomeGrids;
    std::vector<Field> fields;
    Ranges range;

    std::string title;
};
