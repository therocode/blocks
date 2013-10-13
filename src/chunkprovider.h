#pragma once
#include "chunk.h"

class ChunkProvider
{
    public:
        virtual Chunk fetchChunk(const ChunkCoordinate& location) const = 0;       
};
