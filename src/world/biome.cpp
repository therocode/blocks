#include "biome.hpp"

BiomeRequirement::BiomeRequirement(float mi, float ma) : min(mi), max(ma)
{
}

bool BiomeRequirement::isWithin(float v) const
{
    return v >= min && v <= max;
}

Biome::Biome(BiomeId id, VoxelType type, const std::unordered_map<uint32_t, BiomeRequirement>& requirements) : 
    mId(id),
    mType(type),
    mRequirements(requirements)
{
}
