#pragma once
#include <stdint.h>
#include <functional>
#include <vector>
#include "../blockstd.h"

using ChunkCoordinate = glm::ivec3;
using VoxelCoordinate = glm::uvec3;

const int32_t chunkWidth = 16;
const int32_t chunkWidthx2 = chunkWidth * chunkWidth;
const int32_t chunkWidthx3 = chunkWidth * chunkWidth * chunkWidth;
const int32_t voxelAmount = chunkWidthx3;

using VoxelType = uint16_t;
using VoxelTypeArray = std::array<VoxelType, voxelAmount>;

ChunkCoordinate worldToChunk(float x, float y, float z);

ChunkCoordinate worldToChunk(const glm::vec3& position);

VoxelCoordinate worldToVoxel(float x, float y, float z);

VoxelCoordinate worldToVoxel(const glm::vec3& position);

class Chunk
{
    public:
        Chunk(const ChunkCoordinate& loc);
        Chunk(const ChunkCoordinate& loc, const VoxelTypeArray& types);
        void setVoxelType(uint32_t x, uint32_t y, uint32_t z, VoxelType type);
        void setVoxelType(const VoxelCoordinate& voxel, VoxelType type);
        VoxelType getVoxelType(uint32_t x, uint32_t y, uint32_t z) const;
        VoxelType getVoxelType(const VoxelCoordinate& voxel) const;
        VoxelTypeArray& getVoxelTypes();
        const VoxelTypeArray& getVoxelTypes() const;
        uint32_t getWidth() const;
        const ChunkCoordinate& getLocation() const;
        void compress();
    private:
        VoxelTypeArray voxelTypes;
        ChunkCoordinate location;

        bool compressed;
        std::vector<uint16_t> compressedData;
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
