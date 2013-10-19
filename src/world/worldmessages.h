#pragma once
#include "chunk.h"
#include <featherkit/messaging.h>

struct ChunkCreated_tag{};//                               coordinate              chunk
using ChunkCreatedMessage = fea::Message<ChunkCreated_tag, ChunkCoordinate, VoxelTypeArray&>;
