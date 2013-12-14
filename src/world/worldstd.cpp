#include "worldstd.h"

ChunkRegionCoordinate::ChunkRegionCoordinate() : x(0), y(0), z(0)
{
}

ChunkRegionCoordinate::ChunkRegionCoordinate(int8_t xA, int8_t yA, int8_t zA) : x(xA), y(yA), z(zA)
{
}

bool ChunkRegionCoordinate::operator==(const ChunkRegionCoordinate& other) const
{
    return x == other.x && y == other.y && z == other.z;
}

bool ChunkRegionCoordinate::operator!=(const ChunkRegionCoordinate& other) const
{
    return x != other.x || y != other.y || z != other.z;
}

ChunkCoordinate ChunkRegionCoordinate::toWorldSpace(const RegionCoordinate& regionCoordinate)
{
    return ChunkCoordinate(regionCoordinate.x + x, regionCoordinate.y + y, regionCoordinate.z + z);
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

VoxelWorldCoordinate worldToVoxel(const glm::vec3& position)
{
    return worldToVoxel(position.x, position.y, position.z);
}

VoxelCoordinate worldToChunkVoxel(const glm::vec3& position)
{
	return worldToChunkVoxel(position.x, position.y, position.z);
}

VoxelCoordinate worldToChunkVoxel(float x, float y, float z)
{
	int xNegative = (int)(x - chunkWidth) / chunkWidth;
	int yNegative = (int)(y - chunkWidth) / chunkWidth;
	int zNegative = (int)(z - chunkWidth) / chunkWidth;
	
	if(xNegative < 0) x += (-xNegative + 1) * chunkWidth;
	if(yNegative < 0) y += (-yNegative + 1) * chunkWidth;
	if(zNegative < 0) z += (-zNegative + 1) * chunkWidth;

    return VoxelCoordinate((((int)x) % chunkWidth),
                           (((int)y) % chunkWidth),
                           (((int)z) % chunkWidth));
}

VoxelTypeData::VoxelTypeData(const RleIndexArray& rleSegmentIndices, const RleSegmentArray& rleSegments) : mRleSegmentIndices(rleSegmentIndices), mRleSegments(rleSegments)
{

}

RegionCoordinate chunkToRegion(float x, float y, float z)
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
    chunkToRegion(coordinate.x, coordinate.y, coordinate.z);
}
