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
using ChunkVoxelCoord  = glm::u8vec3;

struct SetVoxelMessage
{
    WorldId worldId; const VoxelCoord& voxel; VoxelType type;
};

struct VoxelSetMessage
{
    const VoxelCoord& voxel; VoxelType type;
};

struct RegionRequestedMessage
{
    WorldId worldId; const RegionCoord& coordinate;
};

struct RegionDeliverMessage
{
    WorldId worldId; const RegionCoord& coordinate; const Region& newRegion;
};

struct ChunkRequestedMessage
{
    int32_t prio; WorldId worldId; const ChunkCoord& coordinate; const RegionDataFragment& regionData;
};

struct ChunkDeliverMessage
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
