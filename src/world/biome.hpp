#pragma once
#include <string>
#include "worlddefines.hpp"

struct Range
{
    Range(float mi, float ma);
    bool isWithin(float v) const;
    float min;
    float max;
};

struct Biome
{
    Biome(const std::string& name, VoxelType type);
    std::string mName;
    VoxelType mType;
};
