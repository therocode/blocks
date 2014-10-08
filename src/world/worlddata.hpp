#pragma once
#include "voxelstorage.hpp"
#include "biomedefines.hpp"
#include "biome.hpp"

struct WorldData
{
    VoxelStorage voxels;
    //storage of biomes in indexed list
    std::vector<Biome> biomes;
    BiomeStorage biomeIndices;
    //list of field definitions - rules for them
    //meta with title etc
};
