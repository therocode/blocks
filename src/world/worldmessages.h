#pragma once
#include "worldstd.h"
#include <featherkit/messaging.h>

class Region;
class Chunk;

//struct ChunkCreated_tag{};//                               coordinate              chunk
//using ChunkCreatedMessage = fea::Message<ChunkCreated_tag, const ChunkCoordinate, const RleIndexArray&, const RleSegmentArray&>;

struct SetVoxel_tag{};//
using SetVoxelMessage = fea::Message<SetVoxel_tag, const VoxelWorldCoordinate&, VoxelType>;

struct VoxelSet_tag{};//
using VoxelSetMessage = fea::Message<VoxelSet_tag, const ChunkCoordinate&, const VoxelChunkCoordinate&, VoxelType>;

struct RegionNeeded_tag{};//                               region to force loading of
using RegionNeededMessage = fea::Message<RegionNeeded_tag, const RegionCoordinate&>;

struct RegionDeliver_tag{};//                                coordinate               new region
using RegionDeliverMessage = fea::Message<RegionDeliver_tag, const RegionCoordinate&, const std::reference_wrapper<Region>>;

struct ChunkRequested_tag{};//                                 coordinate
using ChunkRequestedMessage = fea::Message<ChunkRequested_tag, const ChunkCoordinate&>;

struct ChunkDeliver_tag{};//                               coordinate              chunk
using ChunkDeliverMessage = fea::Message<ChunkDeliver_tag, const ChunkCoordinate&, const Chunk&>;

struct ChunkLoaded_tag{};//                              chunk                   timestamp
using ChunkLoadedMessage = fea::Message<ChunkLoaded_tag, const ChunkCoordinate&, uint64_t>;

struct ChunkHighlighted_tag{};//                                   chunk
using ChunkHighlightedMessage = fea::Message<ChunkHighlighted_tag, const ChunkCoordinate&>;

struct ChunkDehighlighted_tag{};//                                     chunk
using ChunkDehighlightedMessage = fea::Message<ChunkDehighlighted_tag, const ChunkCoordinate&>;
