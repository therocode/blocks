#pragma once
#include "chunkdeliverer.h"

class LocalChunkDeliverer : public ChunkDeliverer
{
    public:
        virtual Chunk fetchChunk(const ChunkCoordinate& location) const;
};
