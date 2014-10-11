#include "biome.hpp"

BiomeRequirement::BiomeRequirement(float mi, float ma) : min(mi), max(ma)
{
}

bool BiomeRequirement::isWithin(float v) const
{
    return v >= min && v <= max;
}

Biome::Biome(const std::string& name, VoxelType type, const std::unordered_map<std::string, BiomeRequirement>& requirements) : 
    mName(name),
    mRequirements(requirements)
{
}
