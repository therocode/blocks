#include "worldstd.h"

int64_t wrapInt(int64_t kX, int64_t const kLowerBound, int64_t const kUpperBound)
{
    int64_t range_size = kUpperBound - kLowerBound + 1;

    if (kX < kLowerBound)
        kX += range_size * ((kLowerBound - kX) / range_size + 1);

    return kLowerBound + (kX - kLowerBound) % range_size;
}

ChunkCoordinate worldToChunkInt(int x, int y, int z)
{
	int xNeg = x < 0;
	int yNeg = y < 0;
	int zNeg = z < 0;
	ChunkCoordinate c;
	
	c.x =(((xNeg)?1:0) + x) / chunkWidth;
	c.y =(((yNeg)?1:0) + y) / chunkWidth;
	c.z =(((zNeg)?1:0) + z) / chunkWidth;
	
	if(xNeg) c.x -= 1;
	if(yNeg) c.y -= 1;
	if(zNeg) c.z -= 1;
	
	return c;
}

ChunkCoordinate worldToChunk(float x, float y, float z)
{
	int xNegative = (glm::floor(x) < 0.0f)?1:0;
    int yNegative = (glm::floor(y) < 0.0f)?1:0;
    int zNegative = (glm::floor(z) < 0.0f)?1:0;

    return ChunkCoordinate(	((int)(x) / chunkWidth) - xNegative, 
							((int)(y) / chunkWidth) - yNegative, 
							((int)(z) / chunkWidth) - zNegative);
}

ChunkCoordinate worldToChunk(const glm::vec3& position)
{
    return worldToChunk(position.x, position.y, position.z);
}

VoxelWorldCoordinate worldToVoxel(float x, float y, float z)
{
    return VoxelWorldCoordinate(glm::floor(x),
								glm::floor(y),
								glm::floor(z));
}

//VoxelWorldCoordinate worldToVoxel(const glm::vec3& position)
//{
//    return worldToVoxel(position.x, position.y, position.z);
//}

VoxelChunkCoordinate worldToChunkVoxel(const glm::vec3& position)
{
	return worldToChunkVoxel(position.x, position.y, position.z);
}

VoxelChunkCoordinate worldToChunkVoxel(float x, float y, float z)
{
	int xNegative = (int)(x - chunkWidth) / chunkWidth;
	int yNegative = (int)(y - chunkWidth) / chunkWidth;
	int zNegative = (int)(z - chunkWidth) / chunkWidth;
	
	if(xNegative < 0) x += (-xNegative + 1) * chunkWidth;
	if(yNegative < 0) y += (-yNegative + 1) * chunkWidth;
	if(zNegative < 0) z += (-zNegative + 1) * chunkWidth;

    return VoxelChunkCoordinate((((int)x) % chunkWidth),
                           (((int)y) % chunkWidth),
                           (((int)z) % chunkWidth));
}

VoxelTypeData::VoxelTypeData(const RleIndexArray& rleSegmentIndices, const RleSegmentArray& rleSegments) : mRleSegmentIndices(rleSegmentIndices), mRleSegments(rleSegments)
{

}

RegionCoordinate chunkToRegion(int x, int y, int z)
{
	bool xNeg = x < 0;
	bool yNeg = y < 0;
	bool zNeg = z < 0;
	RegionCoordinate c;
	
	c.x =(((xNeg)?1:0) + x) / regionWidth;
	c.y =(((yNeg)?1:0) + y) / regionWidth;
	c.z =(((zNeg)?1:0) + z) / regionWidth;
	
	if(xNeg) c.x -= 1;
	if(yNeg) c.y -= 1;
	if(zNeg) c.z -= 1;
	
	return c;
}
 
RegionCoordinate chunkToRegion(const ChunkCoordinate& coordinate)
{
    return chunkToRegion(coordinate.x, coordinate.y, coordinate.z);
}

ChunkRegionCoordinate chunkToChunkRegion(int32_t x, int32_t y, int32_t z)
{
    return ChunkRegionCoordinate(wrapInt(x, 0, 31), wrapInt(y, 0, 31), wrapInt(z, 0, 31));
}

ChunkRegionCoordinate chunkToChunkRegion(const ChunkCoordinate& coordinate)
{
    return chunkToChunkRegion(coordinate.x, coordinate.y, coordinate.z);
}

ChunkCoordinate voxelToChunk(const VoxelWorldCoordinate& coordinate)
{
    int32_t x = coordinate.x < 0 ? (coordinate.x / 32) - 1: coordinate.x / 32;
    int32_t y = coordinate.y < 0 ? (coordinate.y / 32) - 1: coordinate.y / 32;
    int32_t z = coordinate.z < 0 ? (coordinate.z / 32) - 1: coordinate.z / 32;

    return ChunkCoordinate(x, y, z);
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
    int64_t x = voxelCoordinate.x < 0 ? (voxelCoordinate.x / chunkWidth) - 1: voxelCoordinate.x / chunkWidth;
    int64_t y = voxelCoordinate.y < 0 ? (voxelCoordinate.y / chunkWidth) - 1: voxelCoordinate.y / chunkWidth;
    int64_t z = voxelCoordinate.z < 0 ? (voxelCoordinate.z / chunkWidth) - 1: voxelCoordinate.z / chunkWidth;

    return ChunkCoord(x, y, z);
}

RegionCoord voxelToRegion(const VoxelCoord& voxelCoordinate)
{
    int32_t width = regionWidth * chunkWidth;
    int64_t x = voxelCoordinate.x < 0 ? (voxelCoordinate.x / width) - 1: voxelCoordinate.x / width;
    int64_t y = voxelCoordinate.y < 0 ? (voxelCoordinate.y / width) - 1: voxelCoordinate.y / width;
    int64_t z = voxelCoordinate.z < 0 ? (voxelCoordinate.z / width) - 1: voxelCoordinate.z / width;

    return RegionCoord(x, y, z);
}

RegionCoord chunkToRegion(const ChunkCoord& chunkCoordinate)
{
    int64_t x = chunkCoordinate.x < 0 ? (chunkCoordinate.x / regionWidth) - 1: chunkCoordinate.x / regionWidth;
    int64_t y = chunkCoordinate.y < 0 ? (chunkCoordinate.y / regionWidth) - 1: chunkCoordinate.y / regionWidth;
    int64_t z = chunkCoordinate.z < 0 ? (chunkCoordinate.z / regionWidth) - 1: chunkCoordinate.z / regionWidth;

    return RegionCoord(x, y, z);
}

ChunkVoxelCoord voxelToChunkVoxel(const VoxelCoord& voxelCoordinate)
{
    int64_t x = wrapInt(voxelCoordinate.x, 0, chunkWidth - 1);
    int64_t y = wrapInt(voxelCoordinate.y, 0, chunkWidth - 1);
    int64_t z = wrapInt(voxelCoordinate.z, 0, chunkWidth - 1);
    
    return ChunkVoxelCoord(x, y, z);
}
