#pragma once
#include <stdint.h>
#include <functional>

class ChunkCoordinate
{
    public:
        bool operator==(const ChunkCoordinate& other) const;
        uint32_t x;
        uint32_t y;
        uint32_t z;
};

const uint32_t chunkWidth = 16;
const uint32_t chunkWidthx2 = chunkWidth * chunkWidth;
const uint32_t chunkWidthx3 = chunkWidth * chunkWidth * chunkWidth;
const uint32_t voxelAmount = chunkWidthx3;

using VoxelTypeArray = std::array<uint16_t, voxelAmount>;

class Chunk
{
    public:
        void setVoxelType(uint32_t x, uint32_t y, uint32_t z, uint16_t type);
        uint16_t getVoxelType(uint32_t x, uint32_t y, uint32_t z) const;
        const VoxelTypeArray& getVoxelTypes() const;
        uint32_t getWidth() const;
    private:
        VoxelTypeArray voxelTypes;
};

namespace std
{
    template<>
        struct hash<ChunkCoordinate>
        {
            public:
                std::size_t operator()(ChunkCoordinate const& coord) const 
                {
                    return coord.x ^ (coord.y ^ (coord.z << 1) << 1);
                }
        };
}
