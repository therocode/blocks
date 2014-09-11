#include "biome.hpp"

Range::Range(float mi, float ma) : min(mi), max(ma)
{
}

bool Range::isWithin(float v) 
{
    return v >= min && v <= max;
}

Biome::Biome(const std::string& name, VoxelType type, float red, float green, float blue, Range temp, Range rain, Range height) : mName(name), mType(type), r(red), g(green), b(blue), temperatureRange(temp), rainfallRange(rain), heightRange(height)
{
}
