#pragma once
#include <string>
#include "../worldconstants.hpp"

struct Range
{
    Range(float mi, float ma);
    bool isWithin(float v) const;
    float min;
    float max;
};

struct Biome
{
    Biome(const std::string& name, VoxelType type, float red, float green, float blue, Range temp, Range rain, Range height);
    std::string mName;
    VoxelType mType;
    float r;
    float g;
    float b;
    Range temperatureRange;
    Range rainfallRange;
    Range heightRange;
};
