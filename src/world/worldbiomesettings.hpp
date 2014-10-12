#pragma once
#include <vector>
#include "field.hpp"
#include "biomedefines.hpp"

struct WorldBiomeSettings
{
    std::vector<Field> fields;
    std::vector<BiomeIndex> biomes;
};
