#pragma once
#include <featherkit/messaging.h>
#include "world/chunk.h"

struct AddGfxEntity_tag{};//                               id      position
using AddGfxEntityMessage = fea::Message<AddGfxEntity_tag, size_t, const glm::vec3&>;

struct MoveGfxEntity_tag{};//                                id      position
using MoveGfxEntityMessage = fea::Message<MoveGfxEntity_tag, size_t, const glm::vec3&>;

struct RotateGfxEntity_tag{};//                                  id      pitch  yaw
using RotateGfxEntityMessage = fea::Message<RotateGfxEntity_tag, size_t, float, float>;

struct RemoveGfxEntity_tag{};//
using RemoveGfxEntityMessage = fea::Message<RemoveGfxEntity_tag, size_t>;

struct CameraUpdated_tag{};//
using CameraUpdatedMessage = fea::Message<CameraUpdated_tag, const glm::vec3&, const glm::vec3&>;

struct PlayerFacingBlock_tag{};//                                playerid    voxelposition
using PlayerFacingBlockMessage = fea::Message<PlayerFacingBlock_tag, size_t, const glm::vec3&>;

struct UpdateChunkVbo_tag{};//                                 main    top     bottom  front   back    left    right
using UpdateChunkVboMessage = fea::Message<UpdateChunkVbo_tag, Chunk*, Chunk*, Chunk*, Chunk*, Chunk*, Chunk*, Chunk*>;
