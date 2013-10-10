#pragma once
#include "input/inputactions.h"
#include <featherkit/messaging.h>

struct ChunkCreated_tag{};//                               coordinate              chunk
using ChunkCreatedMessage = fea::Message<ChunkCreated_tag, const ChunkCoordinate*, const Chunk*>;

struct WindowResize_tag{};//                               width                 height
using WindowResizeMessage = fea::Message<WindowResize_tag, const uint32_t, const uint32_t>;

struct InputAction_tag{};//                              inputaction enum string thing 
using InputActionMessage = fea::Message<InputAction_tag, const InputAction>;
