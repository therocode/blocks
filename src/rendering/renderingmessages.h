#pragma once
#include <fea/util.hpp>
#include "world/chunk.h"

FEA_DECLARE_MESSAGE(ClientChunkDeletedMessage, const ChunkCoord& coordinate;);
FEA_DECLARE_MESSAGE(AddGfxEntityMessage, size_t id; const glm::vec3& position;);
FEA_DECLARE_MESSAGE(MoveGfxEntityMessage, size_t id; const glm::vec3& position;);
FEA_DECLARE_MESSAGE(RotateGfxEntityMessage, size_t id; float pitch; float yaw;);
FEA_DECLARE_MESSAGE(RemoveGfxEntityMessage, size_t id;);
FEA_DECLARE_MESSAGE(CameraUpdatedMessage, const glm::vec3& onevec; const glm::vec3& twovec;);
FEA_DECLARE_MESSAGE(PlayerFacingBlockMessage, size_t playerId; const VoxelCoord& voxelPosition;);
FEA_DECLARE_MESSAGE(UpdateChunkVboMessage, Chunk* main; Chunk* top; Chunk* bottom; Chunk* front; Chunk* back; Chunk* left; Chunk* right;);
