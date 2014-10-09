#pragma once
#include "voxelstorage.hpp"
#include "biomedefines.hpp"
#include "biome.hpp"
#include "field.hpp"

struct WorldData
{
    VoxelStorage voxels;
    std::vector<BiomeIndex> biomes;
    BiomeStorage biomeGrids;
    std::vector<Field> fields;

    std::string title;
};
