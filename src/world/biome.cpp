#include "biome.hpp"

Range::Range(float mi, float ma) : min(mi), max(ma)
{
}

bool Range::isWithin(float v) const
{
    return v >= min && v <= max;
}

Biome::Biome(const std::string& name, VoxelType type) : 
    mName(name)
{
}
