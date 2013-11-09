#pragma once
#include "chunk.h"
#include <featherkit/messaging.h>

struct ChunkCreated_tag{};//                               coordinate              chunk
using ChunkCreatedMessage = fea::Message<ChunkCreated_tag, const ChunkCoordinate, const RleIndexArray&, const RleSegmentArray&>;

struct ChunkDeleted_tag{};//                               coordinate              chunk
using ChunkDeletedMessage = fea::Message<ChunkDeleted_tag, const ChunkCoordinate&>;

struct SetVoxel_tag{};//
using SetVoxelMessage = fea::Message<SetVoxel_tag, const VoxelWorldCoordinate&, VoxelType>;

struct VoxelSet_tag{};//
using VoxelSetMessage = fea::Message<VoxelSet_tag, const ChunkCoordinate&, const VoxelCoordinate&, VoxelType>;
