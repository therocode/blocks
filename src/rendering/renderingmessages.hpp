#pragma once
#include <fea/util.hpp>
#include "world/chunk.hpp"

struct ClientChunkDeletedMessage
{
    const ChunkCoord& coordinate;
};

struct AddGfxEntityMessage
{
    size_t id; const glm::vec3& position;
};

struct MoveGfxEntityMessage
{
    size_t id; const glm::vec3& position;
};

struct RotateGfxEntityMessage
{
    size_t id; float pitch; float yaw;
};

struct RemoveGfxEntityMessage
{
    size_t id;
};

struct CameraUpdatedMessage
{
    const glm::vec3& onevec; const glm::vec3& twovec;
};

struct PlayerFacingBlockMessage
{
    size_t playerId; const VoxelCoord& voxelPosition;
};

struct UpdateChunkVboMessage
{
    Chunk* main; Chunk* top; Chunk* bottom; Chunk* front; Chunk* back; Chunk* left; Chunk* right;
};
