#pragma once
#include "chunkprovider.h"

class LocalChunkProvider : public ChunkProvider
{
    public:
        virtual Chunk fetchChunk(const ChunkCoordinate& location) const;
};
