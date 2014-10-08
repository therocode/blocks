#pragma once
#include "../utilities/glm.hpp"
#include "../utilities/coordinateconverter.hpp"

using WorldId = uint32_t;

using VoxelCoord       = glm::i64vec3;
using ChunkCoord       = glm::i64vec3; 
using ChunkVoxelCoord  = glm::u8vec3;

const int32_t chunkWidth = 16;
const int32_t chunkWidthPow2 = chunkWidth * chunkWidth;
const int32_t chunkWidthPow3 = chunkWidth * chunkWidth * chunkWidth;
const int32_t voxelAmount = chunkWidthPow3;

//top/bottom: y, -y
//left/right:-x,  x
//front/back:-z,  z
enum CubeFace
{
	CUBE_TOP = 0,
	CUBE_BOTTOM,
	CUBE_LEFT,
	CUBE_RIGHT, 
	CUBE_FRONT,
	CUBE_BACK
};

using VoxelType = uint16_t;

ChunkVoxelCoord  voxelToChunkVoxel( const VoxelCoord&  voxelCoordinate);

using ChunkToVoxel =      CoordinateFineConvert<ChunkCoord, VoxelCoord, chunkWidth>;
using WorldToVoxel =      CoordinateCoarseConvert<glm::vec3, VoxelCoord, 1>;
using WorldToChunk =      CoordinateCoarseConvert<glm::vec3, ChunkCoord, chunkWidth>;
using VoxelToWorld =      CoordinateFineConvert<VoxelCoord, glm::vec3, 1>;
using ChunkToWorld =      CoordinateFineConvert<ChunkCoord, glm::vec3, chunkWidth>;
using VoxelToChunk =      CoordinateCoarseConvert<VoxelCoord, ChunkCoord, chunkWidth>;
using VoxelToChunkVoxel = CoordinateWrapConvert<VoxelCoord, ChunkVoxelCoord, 0, chunkWidth - 1>;
