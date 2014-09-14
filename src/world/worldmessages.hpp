#pragma once
#include "worldstd.hpp"
#include <fea/entitysystem.hpp>
#include <fea/util.hpp>

class Region;
class Chunk;

FEA_DECLARE_MESSAGE(SetVoxelMessage, const VoxelCoord& voxel; VoxelType type;);
FEA_DECLARE_MESSAGE(VoxelSetMessage, const VoxelCoord& voxel; VoxelType type;);
FEA_DECLARE_MESSAGE(RegionDeliverMessage, const RegionCoord& coordinate; const Region& newRegion;);
FEA_DECLARE_MESSAGE(RegionDeletedMessage, const RegionCoord& coordinate;);
FEA_DECLARE_MESSAGE(ChunkRequestedMessage, const ChunkCoord& coordinate;);
FEA_DECLARE_MESSAGE(ChunkDeliverMessage, const ChunkCoord& coordinate; const Chunk& chunk;);
FEA_DECLARE_MESSAGE(ChunkDeletedMessage, const ChunkCoord& coordinate;);
FEA_DECLARE_MESSAGE(ChunkLoadedMessage, const ChunkCoord& coordinate; uint64_t timestamp;);
FEA_DECLARE_MESSAGE(ChunkModdedMessage, const Chunk& chunk; uint64_t timestamp;);
FEA_DECLARE_MESSAGE(ChunkHighlightedMessage, const ChunkCoord& coordinate;);
FEA_DECLARE_MESSAGE(ChunkDehighlightedMessage, const ChunkCoord& coordinate;);
FEA_DECLARE_MESSAGE(HaltChunkAndRegionGenerationMessage, const ChunkCoord& chunkCoordinate; const RegionCoord* regionCoordinate;);
