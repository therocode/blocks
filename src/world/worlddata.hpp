#pragma once
#include "chunkmap.hpp"
#include "biomedefines.hpp"
#include "biome.hpp"
#include "worldbiomesettings.hpp"
#include "ranges.hpp"

struct WorldData
{
    ChunkMap voxels;
    BiomeStorage biomeGrids;
    FieldMap fields;
    WorldBiomeSettings biomeSettings;
    Ranges range;

    std::string title;
};
