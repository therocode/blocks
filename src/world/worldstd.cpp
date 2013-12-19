#include "worldstd.h"

int64_t wrapInt(int64_t kX, int64_t const kLowerBound, int64_t const kUpperBound)
{
    int64_t range_size = kUpperBound - kLowerBound + 1;

    if (kX < kLowerBound)
        kX += range_size * ((kLowerBound - kX) / range_size + 1);

    return kLowerBound + (kX - kLowerBound) % range_size;
}

VoxelTypeData::VoxelTypeData(const RleIndexArray& rleSegmentIndices, const RleSegmentArray& rleSegments) : mRleSegmentIndices(rleSegmentIndices), mRleSegments(rleSegments)
{

}

VoxelCoord worldToVoxel(const glm::vec3& worldCoordinate)
{
    return (VoxelCoord)glm::floor(worldCoordinate);
}

glm::vec3 voxelToWorld(const VoxelCoord& voxelCoordinate)
{
    return (glm::vec3) voxelCoordinate;
}

glm::vec3 chunkToWorld(const ChunkCoord& chunkCoordinate)
{
    return ((glm::vec3) chunkCoordinate) * (float)chunkWidth;
}

glm::vec3 regionToWorld(const RegionCoord& regionCoordinate)
{
    return ((glm::vec3) regionCoordinate) * (float)regionWidth;
}

ChunkCoord voxelToChunk(const VoxelCoord& voxelCoordinate)
{
    int64_t x = voxelCoordinate.x < 0 ? ((voxelCoordinate.x+1) / chunkWidth) - 1: voxelCoordinate.x / chunkWidth;
    int64_t y = voxelCoordinate.y < 0 ? ((voxelCoordinate.y+1) / chunkWidth) - 1: voxelCoordinate.y / chunkWidth;
    int64_t z = voxelCoordinate.z < 0 ? ((voxelCoordinate.z+1) / chunkWidth) - 1: voxelCoordinate.z / chunkWidth;

    return ChunkCoord(x, y, z);
}

RegionCoord voxelToRegion(const VoxelCoord& voxelCoordinate)
{
    int32_t width = regionWidth * chunkWidth;
    int64_t x = voxelCoordinate.x < 0 ? ((voxelCoordinate.x+1) / width) - 1: voxelCoordinate.x / width;
    int64_t y = voxelCoordinate.y < 0 ? ((voxelCoordinate.y+1) / width) - 1: voxelCoordinate.y / width;
    int64_t z = voxelCoordinate.z < 0 ? ((voxelCoordinate.z+1) / width) - 1: voxelCoordinate.z / width;

    return RegionCoord(x, y, z);
}

RegionCoord chunkToRegion(const ChunkCoord& chunkCoordinate)
{
    int64_t x = chunkCoordinate.x < 0 ? ((chunkCoordinate.x+1) / regionWidth) - 1: chunkCoordinate.x / regionWidth;
    int64_t y = chunkCoordinate.y < 0 ? ((chunkCoordinate.y+1) / regionWidth) - 1: chunkCoordinate.y / regionWidth;
    int64_t z = chunkCoordinate.z < 0 ? ((chunkCoordinate.z+1) / regionWidth) - 1: chunkCoordinate.z / regionWidth;

    return RegionCoord(x, y, z);
}

ChunkVoxelCoord voxelToChunkVoxel(const VoxelCoord& voxelCoordinate)
{
    int64_t x = wrapInt(voxelCoordinate.x, 0, chunkWidth - 1);
    int64_t y = wrapInt(voxelCoordinate.y, 0, chunkWidth - 1);
    int64_t z = wrapInt(voxelCoordinate.z, 0, chunkWidth - 1);
    
    return ChunkVoxelCoord(x, y, z);
}

RegionChunkCoord voxelToRegionChunk(const VoxelCoord& voxelCoordinate)
{
    ChunkCoord chunkCoordinate = voxelToChunk(voxelCoordinate);
    int64_t x = wrapInt(chunkCoordinate.x, 0, regionWidth - 1);
    int64_t y = wrapInt(chunkCoordinate.y, 0, regionWidth - 1);
    int64_t z = wrapInt(chunkCoordinate.z, 0, regionWidth - 1);
    
    return RegionChunkCoord(x, y, z);
}
