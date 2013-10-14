#include "chunkloadedpackage.h"

ChunkLoadedPackage::ChunkLoadedPackage(const ChunkCoordinate& coordinate, const Chunk& chunk) : Package(typeid(ChunkLoadedPackage)),
                                                                           mCoordinate(coordinate),
                                                                           mChunk(chunk)
{
}

std::vector<uint8_t> ChunkLoadedPackage::serialise() const
{
    //todo
}

void ChunkLoadedPackage::deserialise(std::vector<uint8_t> bytes)
{
    //todo
}
