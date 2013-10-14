#include "chunkloadedpackage.h"

ChunkLoadedPackage::ChunkLoadedPackage(const VoxelTypeArray& voxelTypes) : Package(typeid(ChunkLoadedPackage)),
                                                                           mVoxelTypes(voxelTypes)
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
