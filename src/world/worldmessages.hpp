#pragma once
#include <fea/entitysystem.hpp>
#include <fea/util.hpp>
#include "../utilities/glmhash.hpp"
#include "worlddefines.hpp"
#include "biome.hpp"
#include "biomedefines.hpp"
#include "worldbiomesettings.hpp"
#include "chunk.hpp"

class Region;
class RegionDataFragment;

struct SetVoxelMessage
{
    WorldId worldId; const VoxelCoord& voxel; VoxelType type;
};

struct VoxelSetMessage
{
    WorldId worldId; const VoxelCoord& voxel; VoxelType type;
};

struct WorldBiomeSettingsMessage
{
    WorldId worldId; const WorldBiomeSettings& settings;
};

struct BiomesLoadedMessage
{
    const std::unordered_map<BiomeId, Biome>& biomes;
};

struct BiomeRequestedMessage
{
    WorldId worldId; const BiomeRegionCoord& coordinate;
};

struct BiomeGeneratedMessage
{
    WorldId worldId; const BiomeRegionCoord& coordinate; const BiomeGrid& biomeData; const FieldMap& fields;
};

struct ChunkGenerationRequestedMessage
{
    int32_t prio; WorldId worldId; const ChunkCoord& coordinate; const BiomeGrid& biomeData; const FieldMap& fieldData;
};

struct ChunkGeneratedMessage
{
    WorldId worldId; const ChunkCoord& coordinate; const Chunk& chunk;
};

struct ChunkInitiallyGeneratedMessage
{
    WorldId worldId; const ChunkCoord& coordinate; Chunk& chunk;
};

struct ChunkCandidateMessage
{
    WorldId worldId; const ChunkCoord& coordinate; Chunk& chunk; uint64_t frameNumberDelta;
};

struct ChunkFinishedMessage
{
    WorldId worldId; const ChunkCoord& coordinate; const Chunk& chunk;
};

struct ChunkDeletedMessage
{
    WorldId worldId; const ChunkCoord& coordinate;
};

struct HighlightEntityAddRequestedMessage
{
    WorldId worldId;
    fea::EntityId entityId;
    const ChunkCoord& coordinate;
    uint32_t radius;
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

struct ChunksRequestedMessage
{
    WorldId worldId;
    const std::vector<ChunkCoord>& coordinates;
};

struct ChunksDataDeniedMessage
{
    WorldId worldId;
    const std::vector<ChunkCoord>& coordinates;
};

struct ChunksRefDataDeliveredMessage
{
    WorldId worldId;
    std::unordered_map<ChunkCoord, const Chunk&> chunks;
};

struct ChunksDataDeliveredMessage
{
    WorldId worldId;
    std::unordered_map<ChunkCoord, Chunk> chunks;
};
