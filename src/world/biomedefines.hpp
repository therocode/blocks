#pragma once
#include "../utilities/interpolationgrid3d.hpp"

using BiomeIndex = uint16_t;

const uint32_t biomeRegionWidthInChunks = 16;
const uint32_t biomeRegionWidth = biomeRegionWidthInChunks * chunkWidth;
using BiomeRegionCoord = glm::i64vec3;
using BiomeGrid = InterpolationGrid3D<biomeRegionWidth, biomeRegionWidth, BiomeIndex>; //right now this uses no interpolation, should change when interpolation grid is capable of it
using BiomeStorage = std::unordered_map<BiomeRegionCoord, BiomeGrid>;
