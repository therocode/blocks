#pragma once
#include "chunk.h"

class ChunkDeliverer
{
    public:
        virtual Chunk fetchChunk(const ChunkCoordinate& location) const = 0;       
};
