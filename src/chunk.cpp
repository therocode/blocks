#include "chunk.h"

bool ChunkCoordinate::operator==(const ChunkCoordinate& other) const
{
    return (x == other.x) && (y == other.y) && (z == other.z);
}
