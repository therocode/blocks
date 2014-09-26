#pragma once
#include "../blockstd.hpp"
#include "controllers/movedirection.hpp"
#include "controllers/moveaction.hpp"
#include <fea/util.hpp>
#include <fea/entitysystem.hpp>

using VoxelCoord       = glm::i64vec3;
using ChunkCoord       = glm::i64vec3; 
using RegionCoord      = glm::i64vec2;
using ChunkVoxelCoord  = glm::u8vec3;
using RegionChunkCoord = glm::u8vec3;
using RegionVoxelCoord = glm::u16vec2;

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

struct RemoveEntityMessage
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

struct PlayerConnectedToEntityMessage
{
    size_t playerId; fea::EntityId entityId;
};

struct PlayerEntersChunkMessage
{
    size_t playerId; const ChunkCoord& chunkCoord;
};

struct PlayerIdMessage
{
    size_t id;
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
