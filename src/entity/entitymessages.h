#pragma once
#include "../blockstd.h"
#include "../world/chunk.h"
#include "controllers/movedirection.h"
#include "controllers/moveaction.h"
#include <featherkit/messaging.h>
#include <featherkit/entitysystem.h>

struct EntityMoveRequested_tag{};//                                      id             new position
using EntityMoveRequestedMessage = fea::Message<EntityMoveRequested_tag, fea::EntityId, const glm::vec3&>;

struct EntityMoved_tag{};//                              id             old position      new position
using EntityMovedMessage = fea::Message<EntityMoved_tag, fea::EntityId, const glm::vec3&, const glm::vec3&>;

struct GravityRequested_tag{};//                                   gravity constant
using GravityRequestedMessage = fea::Message<GravityRequested_tag, float>;

struct CreateEntity_tag{};//                               type                position
using CreateEntityMessage = fea::Message<CreateEntity_tag, const std::string&, const glm::vec3&>;

struct EntityCreated_tag{};//                                entity              type
using EntityCreatedMessage = fea::Message<EntityCreated_tag, fea::WeakEntityPtr, const std::string&>;

struct EntityJump_tag{};//                             id      jump or not
using EntityJumpMessage = fea::Message<EntityJump_tag, size_t, const bool>;

struct EntityOnGround_tag{};//                                  id             landed=true, lift off=false
using EntityOnGroundMessage = fea::Message<EntityOnGround_tag, fea::EntityId, bool>;

struct RemoveEntity_tag{};//                               id 
using RemoveEntityMessage = fea::Message<RemoveEntity_tag, size_t>;

struct EntityRemoved_tag{};//                               id 
using EntityRemovedMessage = fea::Message<EntityRemoved_tag, size_t>;

struct PlayerEntity_tag{};//                               player id      player entity
using PlayerEntityMessage = fea::Message<PlayerEntity_tag, size_t, fea::WeakEntityPtr>;

struct PlayerConnectedToEntity_tag{};//                                          player id  entity id
using PlayerConnectedToEntityMessage = fea::Message<PlayerConnectedToEntity_tag, size_t, fea::EntityId>;

struct PlayerEntersChunk_tag{};//                                    playrid      chunk coord
using PlayerEntersChunkMessage = fea::Message<PlayerEntersChunk_tag, size_t, const ChunkCoordinate&>;

struct PlayerId_tag{};//                           id
using PlayerIdMessage = fea::Message<PlayerId_tag, size_t>;

struct PlayerMoveDirection_tag;//                                       id       direction
using PlayerMoveDirectionMessage = fea::Message<PlayerMoveDirection_tag, size_t, const MoveDirection&>;

struct PlayerMoveAction_tag;//                                      id       action
using PlayerMoveActionMessage = fea::Message<PlayerMoveAction_tag, size_t, MoveAction>;

struct PhysicsImpulse_tag{};//                                entity id    force
using PhysicsImpulseMessage = fea::Message<PhysicsImpulse_tag, size_t, const glm::vec3&>;
