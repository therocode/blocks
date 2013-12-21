#pragma once
#include "worldstd.h"
#include <featherkit/entitysystem.h>
#include <featherkit/messaging.h>

class Region;
class Chunk;

//struct ChunkCreated_tag{};//                               coordinate              chunk
//using ChunkCreatedMessage = fea::Message<ChunkCreated_tag, const ChunkCoord, const RleIndexArray&, const RleSegmentArray&>;

struct SetVoxel_tag{};//
using SetVoxelMessage = fea::Message<SetVoxel_tag, const VoxelCoord&, VoxelType>;

struct VoxelSet_tag{};//
using VoxelSetMessage = fea::Message<VoxelSet_tag, const VoxelCoord&, VoxelType>;

struct RegionNeeded_tag{};//                               region to force loading of
using RegionNeededMessage = fea::Message<RegionNeeded_tag, const RegionCoord&>;

struct RegionDeliver_tag{};//                                coordinate               new region
using RegionDeliverMessage = fea::Message<RegionDeliver_tag, const RegionCoord&, const std::reference_wrapper<Region>>;

struct ChunkRequested_tag{};//                                 coordinate
using ChunkRequestedMessage = fea::Message<ChunkRequested_tag, const ChunkCoord&>;

struct ChunkDeliver_tag{};//                               coordinate              chunk
using ChunkDeliverMessage = fea::Message<ChunkDeliver_tag, const ChunkCoord&, const Chunk&>;

struct ChunkLoaded_tag{};//                              chunk loc          timestamp
using ChunkLoadedMessage = fea::Message<ChunkLoaded_tag, const ChunkCoord&, uint64_t>;

struct ChunkModded_tag{};//                              chunk         timestamp
using ChunkModdedMessage = fea::Message<ChunkModded_tag, const Chunk&, uint64_t>;

struct ChunkHighlighted_tag{};//                                   chunk
using ChunkHighlightedMessage = fea::Message<ChunkHighlighted_tag, const ChunkCoord&>;

struct ChunkDehighlighted_tag{};//                                     chunk
using ChunkDehighlightedMessage = fea::Message<ChunkDehighlighted_tag, const ChunkCoord&>;

struct HighlightEntitySpawned_tag{};
using HighlightEntitySpawnedMessage = fea::Message<HighlightEntitySpawned_tag, fea::EntityId, const ChunkCoord&>;

struct HighlightEntityDespawned_tag{};
using HighlightEntityDespawnedMessage = fea::Message<HighlightEntityDespawned_tag, fea::EntityId, const ChunkCoord&>;

struct HighlightEntityMoved_tag{};
using HighlightEntityMovedMessage = fea::Message<HighlightEntityMoved_tag, fea::EntityId, const ChunkCoord&>;
