#pragma once
#include "package.h"
#include "chunk.h"

class ChunkLoadedPackage : public Package
{
    public:
        ChunkLoadedPackage(const VoxelTypeArray& voxelTypes);
        virtual std::vector<uint8_t> serialise() const;
        virtual void deserialise(std::vector<uint8_t> bytes);
        VoxelTypeArray mVoxelTypes;
};
