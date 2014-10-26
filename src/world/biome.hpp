#pragma once
#include <unordered_map>
#include "biomedefines.hpp"
//#include "worlddefines.hpp"

struct BiomeRequirement
{
    BiomeRequirement(float mi, float ma);
    bool isWithin(float v) const;
    float min;
    float max;
};

struct Biome
{
    Biome(BiomeId id, VoxelType type, const std::unordered_map<uint32_t, BiomeRequirement>& requirements);
    BiomeId mId;
    VoxelType mType;
    std::unordered_map<uint32_t, BiomeRequirement> mRequirements;
};
