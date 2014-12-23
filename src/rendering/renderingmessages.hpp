#pragma once
#include <fea/util.hpp>
#include "../world/worlddefines.hpp"

class Chunk;

struct ChunkVBODeletedMessage
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

struct FacingBlockMessage
{
    bool inRange;
    const VoxelCoord& block;
};

struct PlayerFacingBlockMessage
{
    size_t playerId; const VoxelCoord& voxelPosition;
};

struct UpdateChunkVboMessage
{
    const ChunkCoord& mainChunkCoord;
    Chunk* main; Chunk* top; Chunk* bottom; Chunk* front; Chunk* back; Chunk* left; Chunk* right;
};

enum { DISABLE_ALL, ENABLE_ALL, TOGGLE_MODE_ALL, DISABLE_DEBUG, ENABLE_DEBUG, TOGGLE_MODE_DEBUG, DISABLE_MODEL, ENABLE_MODEL, TOGGLE_MODE_MODEL, DISABLE_VOXEL, ENABLE_VOXEL, TOGGLE_MODE_VOXEL };

struct RenderModeMessage
{
    int32_t type;
};
