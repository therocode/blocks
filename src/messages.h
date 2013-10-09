#pragma once
#include <featherkit/messaging.h>

struct ChunkCreated_tag{};//                               coordinate              chunk
using ChunkCreatedMessage = fea::Message<ChunkCreated_tag, const ChunkCoordinate*, const Chunk*>;

struct WindowResize_tag{};//                               width                 height
using WindowResizeMessage = fea::Message<WindowResize_tag, const uint32_t width, const uint32_t height>;
