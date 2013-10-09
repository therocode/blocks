#include "chunk.h"

ChunkCoordinate::ChunkCoordinate(int32_t xCoord, int32_t yCoord, int32_t zCoord) : x(xCoord), y(yCoord), z(zCoord)
{
}

ChunkCoordinate::ChunkCoordinate(const ChunkCoordinate& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
}

bool ChunkCoordinate::operator==(const ChunkCoordinate& other) const
{
    return (x == other.x) && (y == other.y) && (z == other.z);
}

Chunk::Chunk(const ChunkCoordinate& loc) : location(loc)
{
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

const ChunkCoordinate& Chunk::getLocation() const
{
    return location;
}
