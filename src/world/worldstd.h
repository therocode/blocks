#pragma once
#include "../blockstd.h"
#include <vector>
#include <array>

//old
using ChunkCoordinate = glm::ivec3;
using RegionCoordinate = glm::ivec3;
using VoxelChunkCoordinate = glm::uvec3;
using VoxelWorldCoordinate = glm::ivec3;
using ChunkRegionCoordinate = glm::u8vec3;

//new
using VoxelCoord       = glm::i64vec3;
using ChunkCoord       = glm::i64vec3; 
using RegionCoord      = glm::i64vec3;
using ChunkVoxelCoord  = glm::u8vec3;
using RegionChunkCoord = glm::u8vec3;

int64_t wrapInt(int64_t kX, int64_t const kLowerBound, int64_t const kUpperBound);

const int32_t regionWidth = 32;
const int32_t chunkWidth = 16;
const int32_t chunkWidthx2 = chunkWidth * chunkWidth;
const int32_t chunkWidthx3 = chunkWidth * chunkWidth * chunkWidth;
const int32_t voxelAmount = chunkWidthx3;

//top/bottom: y, -y
//left/right:-x,  x
//front/back:-z,  z
enum BLOCK_FACES{
	FACE_TOP = 0,
	FACE_BOTTOM,
	FACE_LEFT,
	FACE_RIGHT, 
	FACE_FRONT,
	FACE_BACK
};

struct RleSegmentInfo
{
    uint32_t mSegmentStart;
    uint32_t mSegmentSize;
};

using VoxelType = uint16_t;
using VoxelTypeArray = std::array<VoxelType, voxelAmount>;
using VoxelSegmentTypeArray = std::array<VoxelType, chunkWidth>;
using RleIndexArray = std::array<RleSegmentInfo, chunkWidthx2>;
using RleSegmentArray = std::vector<uint16_t>;

struct VoxelTypeData
{
    VoxelTypeData(const RleIndexArray& rleSegmentIndices, const RleSegmentArray& rleSegments);
    const RleIndexArray& mRleSegmentIndices;
    const RleSegmentArray& mRleSegments;    
};

//old
ChunkCoordinate worldToChunk(float x, float y, float z);
ChunkCoordinate worldToChunk(const glm::vec3& position);
ChunkCoordinate worldToChunkInt(int x, int y, int z);

VoxelChunkCoordinate worldToChunkVoxel(const glm::vec3& position);
VoxelChunkCoordinate worldToChunkVoxel(float x, float y, float z);

VoxelWorldCoordinate worldToVoxel(float x, float y, float z);
//VoxelWorldCoordinate worldToVoxel(const glm::vec3& position);

RegionCoordinate chunkToRegion(int x, int y, int z);
RegionCoordinate chunkToRegion(const ChunkCoordinate& coordinate);

ChunkRegionCoordinate chunkToChunkRegion(int32_t x, int32_t y, int32_t z);
ChunkRegionCoordinate chunkToChunkRegion(const ChunkCoordinate& coordinate);

ChunkCoordinate voxelToChunk(const VoxelWorldCoordinate& coordinate);

//new
VoxelCoord worldToVoxel(const glm::vec3& worldCoordinate);
glm::vec3  voxelToWorld(const VoxelCoord& voxelCoordinate);
glm::vec3  chunkToWorld(const ChunkCoord& chunkCoordinate);
glm::vec3  regionToWorld(const RegionCoord& regionCoordinate);
ChunkCoord voxelToChunk(const VoxelCoord& voxelCoordinate);
RegionCoord voxelToRegion(const VoxelCoord& voxelCoordinate);
RegionCoord chunkToRegion(const ChunkCoord& chunkCoordinate);
ChunkVoxelCoord voxelToChunkVoxel(const VoxelCoord& voxelCoordinate);
