#pragma once
#include "../world/worlddefines.hpp"

struct ClientChunkDeletedMessage
{
    const ChunkCoord& coordinate;
};
