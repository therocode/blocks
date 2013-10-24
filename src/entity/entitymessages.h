#pragma once
#include "../blockstd.h"
#include "../world/chunk.h"
#include <featherkit/messaging.h>
#include <featherkit/entitysystem.h>

struct EntityMoveRequested_tag{};//                                      id             new position
using EntityMoveRequestedMessage = fea::Message<EntityMoveRequested_tag, fea::EntityId, const glm::vec3&>;

struct EntityMoved_tag{};//                              id             old position      new position
using EntityMovedMessage = fea::Message<EntityMoved_tag, fea::EntityId, const glm::vec3&, const glm::vec3&>;

struct GravityRequested_tag{};//                                   gravity constant
using GravityRequestedMessage = fea::Message<GravityRequested_tag, float>;

struct SpawnEntity_tag{};//                              script type         position
using SpawnEntityMessage = fea::Message<SpawnEntity_tag, const std::string&, const glm::vec3&>;

struct EntityNeedsScript_tag{};//                                entity              //script type
using EntityNeedsScriptMessage = fea::Message<EntityNeedsScript_tag, fea::WeakEntityPtr, const std::string&>;

struct EntitySpawned_tag{};//                                entity              //script type
using EntitySpawnedMessage = fea::Message<EntitySpawned_tag, fea::WeakEntityPtr, const std::string&>;

class asIScriptObject;
struct ScriptEntityFinished_tag{};//             id      obj
using ScriptEntityFinishedMessage = fea::Message<ScriptEntityFinished_tag, size_t, asIScriptObject*, fea::WeakEntityPtr>;

struct RemoveEntity_tag{};//                               id 
using RemoveEntityMessage = fea::Message<RemoveEntity_tag, size_t>;

struct RemoveScriptEntity_tag{};//                                     obj 
using RemoveScriptEntityMessage = fea::Message<RemoveScriptEntity_tag, asIScriptObject*>;

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
