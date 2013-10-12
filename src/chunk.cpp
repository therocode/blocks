#include "chunk.h"

ChunkCoordinate worldToChunk(float x, float y, float z)
{
    return ChunkCoordinate(floor(x / (float)chunkWidth), floor(y / (float)chunkWidth), floor(z / (float)chunkWidth));
}

ChunkCoordinate worldToChunk(const glm::vec3& position)
{
    return worldToChunk(position.x, position.y, position.z);
}

VoxelCoordinate worldToVoxel(float x, float y, float z)
{
    bool xNegative = x < 0.0f;
    bool yNegative = y < 0.0f;
    bool zNegative = z < 0.0f;

    return VoxelCoordinate((((int)x) % chunkWidth) + (xNegative?chunkWidth-1:0),
                           (((int)y) % chunkWidth) + (yNegative?chunkWidth-1:0),
                           (((int)z) % chunkWidth) + (zNegative?chunkWidth-1:0));
}

VoxelCoordinate worldToVoxel(const glm::vec3& position)
{
    return worldToVoxel(position.x, position.y, position.z);
}

Chunk::Chunk(const ChunkCoordinate& loc) : location(loc)
{
}

void Chunk::setVoxelType(uint32_t x, uint32_t y, uint32_t z, VoxelType type)
{
    voxelTypes[x + y * chunkWidth + z * chunkWidthx3] = type;
}

VoxelType Chunk::getVoxelType(uint32_t x, uint32_t y, uint32_t z) const
{
    return voxelTypes[x + y * chunkWidth + z * chunkWidthx3];
}

VoxelTypeArray& Chunk::getVoxelTypes()
{
    return voxelTypes;
}

const VoxelTypeArray& Chunk::getVoxelTypes() const
{
    return voxelTypes;
}

uint32_t Chunk::getWidth() const
{
    return chunkWidth;
}

const ChunkCoordinate& Chunk::getLocation() const
{
    return location;
}
