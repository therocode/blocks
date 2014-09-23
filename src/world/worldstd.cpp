#include "worldstd.hpp"

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

VoxelCoord chunkToVoxel(const ChunkCoord& chunkCoordinate)
{
    return (VoxelCoord)(chunkCoordinate * (int64_t)chunkWidth);
}

VoxelCoord worldToVoxel(const glm::vec3& worldCoordinate)
{
    return (VoxelCoord)glm::floor(worldCoordinate);
}

ChunkCoord worldToChunk(const glm::vec3& worldCoordinate)
{
    return (ChunkCoord)(glm::floor(worldCoordinate / (float)chunkWidth));
}

RegionCoord worldToRegion(const glm::vec3& worldCoordinate)
{
    return (RegionCoord)(glm::floor(worldCoordinate / (float)(regionVoxelWidth)));
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
    glm::vec2 worldCoordinate = ((glm::vec2) regionCoordinate) * (float)(regionVoxelWidth);
    return glm::vec3(worldCoordinate.x, 0.0f, worldCoordinate.y);
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
    int32_t width = regionVoxelWidth;
    int64_t x = voxelCoordinate.x < 0 ? ((voxelCoordinate.x+1) / width) - 1: voxelCoordinate.x / width;
    int64_t z = voxelCoordinate.z < 0 ? ((voxelCoordinate.z+1) / width) - 1: voxelCoordinate.z / width;

    return RegionCoord(x, z);
}

RegionCoord chunkToRegion(const ChunkCoord& chunkCoordinate)
{
    int64_t x = chunkCoordinate.x < 0 ? ((chunkCoordinate.x+1) / regionChunkWidth) - 1: chunkCoordinate.x / regionChunkWidth;
    int64_t z = chunkCoordinate.z < 0 ? ((chunkCoordinate.z+1) / regionChunkWidth) - 1: chunkCoordinate.z / regionChunkWidth;

    return RegionCoord(x, z);
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
    int64_t x = wrapInt(chunkCoordinate.x, 0, regionChunkWidth - 1);
    int64_t y = wrapInt(chunkCoordinate.y, 0, regionChunkWidth - 1);
    int64_t z = wrapInt(chunkCoordinate.z, 0, regionChunkWidth - 1);
    
    return RegionChunkCoord(x, y, z);
}

RegionChunkCoord chunkToRegionChunk(const ChunkCoord& chunkCoordinate)
{
    int64_t x = wrapInt(chunkCoordinate.x, 0, regionChunkWidth - 1);
    int64_t y = wrapInt(chunkCoordinate.y, 0, regionChunkWidth - 1);
    int64_t z = wrapInt(chunkCoordinate.z, 0, regionChunkWidth - 1);

    return RegionChunkCoord(x, y, z);
}

RegionVoxelCoord voxelToRegionVoxel(const VoxelCoord& voxelCoordinate)
{
    int32_t width = regionVoxelWidth;
    int64_t x = wrapInt(voxelCoordinate.x, 0, width - 1);
    int64_t z = wrapInt(voxelCoordinate.z, 0, width - 1);

    return RegionVoxelCoord(x, z);
}
