#pragma once
#include "package.h"
#include "chunk.h"

class ChunkLoadedPackage : public BasePackage
{
    public:
        ChunkLoadedPackage(const ChunkCoordinate& coordinate, const Chunk& chunk);
        virtual std::vector<uint8_t> serialise() const;
        virtual void deserialise(std::vector<uint8_t> bytes);
        ChunkCoordinate mCoordinate;
        Chunk mChunk;
};
