#pragma once
#include <string>
#include "worldstd.h"

struct Range
{
    Range(float mi, float ma);
    bool isWithin(float v);
    float min;
    float max;
};

struct Biome
{
    Biome(const std::string& name, VoxelType type, float red, float green, float blue, Range temp, Range rain, Range height);
    VoxelType mType;
    float r;
    float g;
    float b;
    Range temperatureRange;
    Range rainfallRange;
    Range heightRange;
};
