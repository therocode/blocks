#pragma once
#include <unordered_map>
#include "worlddefines.hpp"
#include "../utilities/interpolationgrid3d.hpp"

using BiomeIndex = uint16_t;

const uint32_t biomeRegionWidthInChunks = 16;
const uint32_t biomeRegionWidth = biomeRegionWidthInChunks * chunkWidth;
const uint32_t biomeDownSamplingAmount = 8;
using BiomeRegionCoord = glm::i64vec3;
using BiomeRegionChunkCoord = glm::u8vec3;
using BiomeGrid = InterpolationGrid3D<BiomeIndex>; //right now this uses no interpolation, should change when interpolation grid is capable of it
using BiomeStorage = std::unordered_map<BiomeRegionCoord, BiomeGrid>;

using ChunkToBiomeRegion = CoordinateCoarseConvert<ChunkCoord, BiomeRegionCoord, biomeRegionWidthInChunks>;
using ChunkToBiomeRegionChunk = CoordinateWrapConvert<ChunkCoord, BiomeRegionChunkCoord, 0, biomeRegionWidthInChunks - 1>;
