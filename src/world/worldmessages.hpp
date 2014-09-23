#pragma once
#include <fea/entitysystem.hpp>
#include <fea/util.hpp>

class Region;
class Chunk;
using VoxelCoord       = glm::i64vec3;
using ChunkCoord       = glm::i64vec3; 
using RegionCoord      = glm::i64vec2;
using ChunkVoxelCoord  = glm::u8vec3;
using RegionChunkCoord = glm::u8vec3;
using RegionVoxelCoord = glm::u16vec2;

FEA_DECLARE_MESSAGE(SetVoxelMessage, WorldId worldId; const VoxelCoord& voxel; VoxelType type;);
FEA_DECLARE_MESSAGE(VoxelSetMessage, const VoxelCoord& voxel; VoxelType type;);
FEA_DECLARE_MESSAGE(RegionRequestedMessage, WorldId worldId; const RegionCoord& coordinate;);
FEA_DECLARE_MESSAGE(RegionDeliverMessage, WorldId worldId; const RegionCoord& coordinate; const Region& newRegion;);
FEA_DECLARE_MESSAGE(RegionDeletedMessage, WorldId worldId; const RegionCoord& coordinate;);
FEA_DECLARE_MESSAGE(ChunkRequestedMessage, int32_t prio; WorldId worldId; const ChunkCoord& coordinate;);
FEA_DECLARE_MESSAGE(ChunkDeliverMessage, WorldId worldId; const ChunkCoord& coordinate; const Chunk& chunk;);
FEA_DECLARE_MESSAGE(ChunkDeletedMessage, const ChunkCoord& coordinate;);
FEA_DECLARE_MESSAGE(ChunkLoadedMessage, const Chunk& chunk; uint64_t timestamp;);
FEA_DECLARE_MESSAGE(ChunkModdedMessage, const Chunk& chunk; uint64_t timestamp;);
FEA_DECLARE_MESSAGE(HaltChunkAndRegionGenerationMessage, WorldId worldId; const ChunkCoord& chunkCoordinate; const RegionCoord* regionCoordinate;);
