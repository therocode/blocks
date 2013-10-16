#pragma once
#include "input/inputactions.h"
#include "chunk.h"
#include "blockstd.h"
#include <featherkit/messaging.h>
#include <featherkit/entitysystem.h>

struct ChunkCreated_tag{};//                               coordinate              chunk
using ChunkCreatedMessage = fea::Message<ChunkCreated_tag, const ChunkCoordinate*, const Chunk*>;

struct WindowResize_tag{};//                               width                 height
using WindowResizeMessage = fea::Message<WindowResize_tag, const uint32_t, const uint32_t>;

struct InputAction_tag{};//                              inputaction enum string thing 
using InputActionMessage = fea::Message<InputAction_tag, const InputAction>;

struct MouseMoved_tag{};//                                moveX         moveY
using MouseMovedMessage = fea::Message<MouseMoved_tag, const float, const float>;

struct EntityMoveRequested_tag{};//                                      id             new position
using EntityMoveRequestedMessage = fea::Message<EntityMoveRequested_tag, fea::EntityId, const glm::vec3&>;

struct EntityMoved_tag{};//                              id             old position      new position
using EntityMovedMessage = fea::Message<EntityMoved_tag, fea::EntityId, const glm::vec3&, const glm::vec3&>;

struct AddGfxEntity_tag{};//                               id      position
using AddGfxEntityMessage = fea::Message<AddGfxEntity_tag, size_t, const glm::vec3&>;

struct MoveGfxEntity_tag{};//                                id      position
using MoveGfxEntityMessage = fea::Message<MoveGfxEntity_tag, size_t, const glm::vec3&>;

struct RemoveGfxEntity_tag{};//
using RemoveGfxEntityMessage = fea::Message<RemoveGfxEntity_tag, size_t>;

struct CameraUpdated_tag{};//
using CameraUpdatedMessage = fea::Message<CameraUpdated_tag, const glm::vec3&, const glm::vec3&>;

struct CurrentlyFacingBlock_tag{};
using CurrentlyFacingBlockMessage = fea::Message<CurrentlyFacingBlock_tag, const glm::vec3&>;

struct Frame_tag{};//                                       true if beginning of frame, false if ending of frame
using FrameMessage = fea::Message<CurrentlyFacingBlock_tag, bool>;

struct RebuildScriptsRequested_tag{};//
using RebuildScriptsRequestedMessage = fea::Message<RebuildScriptsRequested_tag>;

struct GravityRequested_tag{};//                                   gravity constant
using GravityRequestedMessage = fea::Message<GravityRequested_tag, float>;
