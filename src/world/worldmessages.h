#pragma once
#include "chunk.h"
#include <featherkit/messaging.h>

//struct ChunkCreated_tag{};//                               coordinate              chunk
//using ChunkCreatedMessage = fea::Message<ChunkCreated_tag, const ChunkCoordinate, const RleIndexArray&, const RleSegmentArray&>;

struct SetVoxel_tag{};//
using SetVoxelMessage = fea::Message<SetVoxel_tag, const VoxelWorldCoordinate&, VoxelType>;

struct VoxelSet_tag{};//
using VoxelSetMessage = fea::Message<VoxelSet_tag, const ChunkCoordinate&, const VoxelCoordinate&, VoxelType>;

struct ChunkRequested_tag{};//                                 coordinate
using ChunkRequestedMessage = fea::Message<ChunkRequested_tag, const ChunkCoordinate&>;

struct ChunkLoaded_tag{};//                              chunk                   timestamp
using ChunkLoadedMessage = fea::Message<ChunkLoaded_tag, const ChunkCoordinate&, uint64_t>;

struct DeliverChunk_tag{};//                               coordinate              chunk
using DeliverChunkMessage = fea::Message<DeliverChunk_tag, const ChunkCoordinate&, const Chunk&>;

struct RegionNeeded_tag{};//                               region to force loading of
using RegionNeededMessage = fea::Message<RegionNeeded_tag, const RegionCoordinate&>;
