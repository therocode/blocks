#pragma once
#include "../blockstd.hpp"
#include "../world/chunk.hpp"
#include "controllers/movedirection.hpp"
#include "controllers/moveaction.hpp"
#include <fea/util.hpp>
#include <fea/entitysystem.hpp>

FEA_DECLARE_MESSAGE(EntityMoveRequestedMessage, fea::EntityId id; const glm::vec3& newPosition;);
FEA_DECLARE_MESSAGE(EntityMovedMessage, fea::EntityId id; const glm::vec3& oldPosition; const glm::vec3& newPosition;);
FEA_DECLARE_MESSAGE(GravityRequestedMessage, float gravityConstant;);
FEA_DECLARE_MESSAGE(CreateEntityMessage, const std::string& type; std::function<void(fea::EntityPtr)> initializer;);
FEA_DECLARE_MESSAGE(EntityCreatedMessage, fea::WeakEntityPtr entity; const std::string& type;);
FEA_DECLARE_MESSAGE(EntityJumpMessage, size_t id; const bool jumping;);
FEA_DECLARE_MESSAGE(EntityOnGroundMessage, fea::EntityId id; bool landed;);
FEA_DECLARE_MESSAGE(RemoveEntityMessage, size_t id;);
FEA_DECLARE_MESSAGE(EntityRemovedMessage, size_t id;);
FEA_DECLARE_MESSAGE(PlayerEntityMessage, size_t playerId; fea::WeakEntityPtr playerEntity;);
FEA_DECLARE_MESSAGE(PlayerConnectedToEntityMessage, size_t playerId; fea::EntityId entityId;);
FEA_DECLARE_MESSAGE(PlayerEntersChunkMessage, size_t playerId; const ChunkCoord& chunkCoord;);
FEA_DECLARE_MESSAGE(PlayerIdMessage, size_t id;);
FEA_DECLARE_MESSAGE(PlayerMoveDirectionMessage, size_t id; const MoveDirection& direction;);
FEA_DECLARE_MESSAGE(PlayerMoveActionMessage, size_t id; MoveAction action;);
FEA_DECLARE_MESSAGE(PhysicsImpulseMessage, size_t id; const glm::vec3& force;);
