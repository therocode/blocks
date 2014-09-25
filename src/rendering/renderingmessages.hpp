#pragma once
#include <fea/util.hpp>

class Chunk;
using VoxelCoord       = glm::i64vec3;
using ChunkCoord       = glm::i64vec3; 
using RegionCoord      = glm::i64vec2;
using ChunkVoxelCoord  = glm::u8vec3;
using RegionChunkCoord = glm::u8vec3;
using RegionVoxelCoord = glm::u16vec2;

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
