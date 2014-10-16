#pragma once
#include "../utilities/glm.hpp"
#include "controllers/movedirection.hpp"
#include "controllers/moveaction.hpp"
#include <fea/util.hpp>
#include <fea/entitysystem.hpp>
#include "../input/inputactions.hpp"
#include "../world/worlddefines.hpp"

struct EntityMoveRequestedMessage
{
    fea::EntityId id; const glm::vec3& newPosition;
};

struct EntityMovedMessage
{
    fea::EntityId id; const glm::vec3& oldPosition; const glm::vec3& newPosition;
};

struct GravityRequestedMessage
{
    float gravityConstant;
};

struct EntityRequestedMessage
{
    const std::string& type; std::function<void(fea::EntityPtr)> initializer;
};

struct EntityCreatedMessage
{
    fea::WeakEntityPtr entity; const std::string& type;
};

struct EntityJumpMessage
{
    size_t id; const bool jumping;
};

struct EntityOnGroundMessage
{
    fea::EntityId id; bool landed;
};

struct RemoveEntityRequestedMessage
{
    size_t id;
};

struct EntityRemovedMessage
{
    size_t id;
};

struct PlayerEntityMessage
{
    size_t playerId; fea::WeakEntityPtr playerEntity;
};

struct PlayerEntityMovedMessage
{
    size_t playerId; const glm::vec3& position;
};

struct PlayerEntersChunkMessage
{
    size_t playerId; const ChunkCoord& chunkCoord;
};

struct PlayerEntersWorldMessage
{
    size_t playerId; WorldId worldId;
};

struct PlayerAttachedToEntityMessage
{
    size_t playerId;
    fea::EntityId entityId;
    fea::WeakEntityPtr entity;
};

struct PlayerActionMessage
{
    size_t playerId;
    InputAction action;
};

struct PlayerMoveDirectionMessage
{
    size_t id; const MoveDirection& direction;
};

struct PlayerMoveActionMessage
{
    size_t id; MoveAction action;
};

struct PhysicsImpulseMessage
{
    size_t id; const glm::vec3& force;
};
