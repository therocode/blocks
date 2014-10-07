#pragma once
#include "utilities/glm.hpp"

using WorldId = uint32_t;

using VoxelCoord       = glm::i64vec3;
using ChunkCoord       = glm::i64vec3; 
using RegionCoord      = glm::i64vec2;
using ChunkVoxelCoord  = glm::u8vec3;
using RegionChunkCoord = glm::u8vec3;
using RegionVoxelCoord = glm::u16vec2;

int64_t wrapInt(int64_t kX, int64_t const kLowerBound, int64_t const kUpperBound);

const int32_t chunkWidth = 16;
const int32_t regionChunkWidth = 32;
const int32_t regionChunkWidthPow2 = regionChunkWidth * regionChunkWidth;
const int32_t regionVoxelWidth = regionChunkWidth * chunkWidth;
const int32_t regionVoxelWidthPow2 = regionVoxelWidth * regionVoxelWidth;
const int32_t chunkWidthPow2 = chunkWidth * chunkWidth;
const int32_t chunkWidthPow3 = chunkWidth * chunkWidth * chunkWidth;
const int32_t voxelAmount = chunkWidthPow3;

//top/bottom: y, -y
//left/right:-x,  x
//front/back:-z,  z
enum CUBE_FACES{
	CUBE_TOP = 0,
	CUBE_BOTTOM,
	CUBE_LEFT,
	CUBE_RIGHT, 
	CUBE_FRONT,
	CUBE_BACK
};

using VoxelType = uint16_t;

VoxelCoord       chunkToVoxel(      const ChunkCoord& chunkCoordinate);
VoxelCoord       worldToVoxel(      const glm::vec3&   worldCoordinate);
ChunkCoord       worldToChunk(      const glm::vec3&   worldCoordinate);
RegionCoord      worldToRegion(     const glm::vec3&   worldCoordinate);
glm::vec3        voxelToWorld(      const VoxelCoord&  voxelCoordinate);
glm::vec3        chunkToWorld(      const ChunkCoord&  chunkCoordinate);
glm::vec3        regionToWorld(     const RegionCoord& regionCoordinate);
ChunkCoord       voxelToChunk(      const VoxelCoord&  voxelCoordinate);
RegionCoord      voxelToRegion(     const VoxelCoord&  voxelCoordinate);
RegionCoord      chunkToRegion(     const ChunkCoord&  chunkCoordinate);
ChunkVoxelCoord  voxelToChunkVoxel( const VoxelCoord&  voxelCoordinate);
RegionChunkCoord voxelToRegionChunk(const VoxelCoord&  voxelCoordinate);
RegionChunkCoord chunkToRegionChunk(const ChunkCoord&  chunkCoordinate);
RegionVoxelCoord voxelToRegionVoxel(const VoxelCoord&  voxelCoordinate);
