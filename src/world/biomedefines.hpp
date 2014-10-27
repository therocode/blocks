#pragma once
#include <unordered_map>
#include "worlddefines.hpp"
#include "../utilities/interpolationgrid3d.hpp"
#include "field.hpp"

using BiomeId = uint16_t;

const uint32_t biomeRegionWidthInChunks = 16;
const uint32_t biomeRegionWidth = biomeRegionWidthInChunks * chunkWidth;
const uint32_t biomeDownSamplingAmount = 3;
using BiomeRegionCoord = glm::i64vec3;
using BiomeRegionChunkCoord = glm::u8vec3;
using BiomeGrid = InterpolationGrid3D<BiomeId>; //right now this uses no interpolation, should change when interpolation grid is capable of it
using BiomeStorage = std::unordered_map<BiomeRegionCoord, BiomeGrid>;

using FieldMap = std::unordered_map<uint32_t, FieldGrid>;
using FieldStorage = std::unordered_map<BiomeRegionCoord, FieldMap>;

using ChunkToBiomeRegion = CoordinateCoarseConvert<ChunkCoord, BiomeRegionCoord, biomeRegionWidthInChunks>;
using ChunkToBiomeRegionChunk = CoordinateWrapConvert<ChunkCoord, BiomeRegionChunkCoord, 0, biomeRegionWidthInChunks - 1>;
