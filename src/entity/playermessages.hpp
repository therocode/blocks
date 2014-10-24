#pragma once
#include <fea/entitysystem.hpp>
#include "../input/inputactions.hpp"

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
