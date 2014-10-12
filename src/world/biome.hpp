#pragma once
#include <string>
#include <unordered_map>
#include "worlddefines.hpp"

struct BiomeRequirement
{
    BiomeRequirement(float mi, float ma);
    bool isWithin(float v) const;
    float min;
    float max;
};

struct Biome
{
    Biome(const std::string& name, VoxelType type, const std::unordered_map<std::string, BiomeRequirement>& requirements);
    std::string mName;
    VoxelType mType;
    std::unordered_map<std::string, BiomeRequirement> mRequirements;
};
