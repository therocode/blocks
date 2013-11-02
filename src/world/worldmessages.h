#pragma once
#include "chunk.h"
#include <featherkit/messaging.h>

struct ChunkCreated_tag{};//                               coordinate              chunk
using ChunkCreatedMessage = fea::Message<ChunkCreated_tag, const ChunkCoordinate, std::array<uint32_t, chunkWidthx2>, std::vector<uint16_t> >;

struct ChunkDeleted_tag{};//                               coordinate              chunk
using ChunkDeletedMessage = fea::Message<ChunkDeleted_tag, const ChunkCoordinate&>;
