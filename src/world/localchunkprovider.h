#pragma once
#include "chunkprovider.h"
#include "utilities/noise.h"

class LocalChunkProvider : public ChunkProvider
{
    public:
        LocalChunkProvider();
        virtual Chunk fetchChunk(const ChunkCoordinate& location) const;
    private:
        Noise mNoise;
};
