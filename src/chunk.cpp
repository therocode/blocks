#include "chunk.h"

bool ChunkCoordinate::operator==(const ChunkCoordinate& other) const
{
    return (x == other.x) && (y == other.y) && (z == other.z);
}

void Chunk::setVoxelType(uint32_t x, uint32_t y, uint32_t z, uint16_t type)
{
    voxelTypes[x + y * chunkWidth + z * chunkWidthx3] = type;
}

uint16_t Chunk::getVoxelType(uint32_t x, uint32_t y, uint32_t z) const
{
    return voxelTypes[x + y * chunkWidth + z * chunkWidthx3];
}

const VoxelTypeArray& Chunk::getVoxelTypes() const
{
    return voxelTypes;
}

uint32_t Chunk::getWidth() const
{
    return chunkWidth;
}
