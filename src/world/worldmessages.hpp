#pragma once
#include <fea/entitysystem.hpp>
#include <fea/util.hpp>

class Region;
class Chunk;
class RegionDataFragment;

using WorldId = uint32_t;
using VoxelType = uint16_t;
using VoxelCoord       = glm::i64vec3;
using ChunkCoord       = glm::i64vec3; 
using RegionCoord      = glm::i64vec2;
using ChunkVoxelCoord  = glm::u8vec3;
using RegionChunkCoord = glm::u8vec3;
using RegionVoxelCoord = glm::u16vec2;

struct SetVoxelMessage
{
    WorldId worldId; const VoxelCoord& voxel; VoxelType type;
};

struct VoxelSetMessage
{
    const VoxelCoord& voxel; VoxelType type;
};

struct RegionGenerationRequestedMessage
{
    WorldId worldId; const RegionCoord& coordinate;
};

struct RegionGeneratedMessage
{
    WorldId worldId; const RegionCoord& coordinate; const Region& newRegion;
};

struct ChunkGenerationRequestedMessage
{
    int32_t prio; WorldId worldId; const ChunkCoord& coordinate; const RegionDataFragment& regionData;
};

struct ChunkGeneratedMessage
{
    WorldId worldId; const ChunkCoord& coordinate; const Chunk& chunk;
};

struct ChunkDeletedMessage
{
    const ChunkCoord& coordinate;
};

struct ChunkLoadedMessage
{
    const Chunk& chunk; uint64_t timestamp;
};

struct ChunkModdedMessage
{
    const Chunk& chunk; uint64_t timestamp;
};

struct HighlightEntityAddRequestedMessage
{
    WorldId worldId;
    fea::EntityId entityId;
    const ChunkCoord& coordinate;
};

struct HighlightEntityMoveRequestedMessage
{
    WorldId worldId;
    fea::EntityId entityId;
    const ChunkCoord& coordinate;
};

struct HighlightEntityRemoveRequestedMessage
{
    WorldId worldId;
    fea::EntityId entityId;
};
