#pragma once
#include <fea/entitysystem.hpp>
#include <fea/util.hpp>
#include "worldconstants.hpp"

class Region;
class Chunk;
class RegionDataFragment;

struct SetVoxelMessage
{
    WorldId worldId; const VoxelCoord& voxel; VoxelType type;
};

struct VoxelSetMessage
{
    const VoxelCoord& voxel; VoxelType type;
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
