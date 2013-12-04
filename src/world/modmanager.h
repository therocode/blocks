#pragma once

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "chunk.h"

#define PR(x) std::cerr << #x << " = " << (x) << std::endl;

#define NO_CHUNK -1

extern const std::string REGION_DIR;
extern const uint8_t REGION_LENGTH;

namespace std
{
    template<>
        struct hash<VoxelCoordinate>
        {
            public:
                std::size_t operator()(VoxelCoordinate const& coord) const 
                {
                    size_t x = coord.x;
                    x = (x | (x << 32)) & 0xFFFF00000000FFFF;
                    x = (x | (x << 16)) & 0x00FF0000FF0000FF;
                    x = (x | (x << 8))  & 0xF00F00F00F00F00F;
                    x = (x | (x << 4))  & 0x30C30C30C30C30C3;
                    x = (x | (x << 2))  & 0x9249249249249249;

                    size_t y = coord.y;
                    y = (y | (y << 32)) & 0xFFFF00000000FFFF;
                    y = (y | (y << 16)) & 0x00FF0000FF0000FF;
                    y = (y | (y << 8))  & 0xF00F00F00F00F00F;
                    y = (y | (y << 4))  & 0x30C30C30C30C30C3;
                    y = (y | (y << 2))  & 0x9249249249249249;

                    size_t z = coord.z;
                    z = (z | (z << 32)) & 0xFFFF00000000FFFF;
                    z = (z | (z << 16)) & 0x00FF0000FF0000FF;
                    z = (z | (z << 8))  & 0xF00F00F00F00F00F;
                    z = (z | (z << 4))  & 0x30C30C30C30C30C3;
                    z = (z | (z << 2))  & 0x9249249249249249;

                    return x | (y << 1) | (z << 2);
                }
        };
}

struct VoxelCoordinate_uint8
{
    uint8_t x, y, z;

    VoxelCoordinate_uint8();
    VoxelCoordinate_uint8(VoxelCoordinate coord);

    VoxelCoordinate toVoxelCoordinate()
    {
        return VoxelCoordinate(x, y, z);
    }
};

struct Mod
{
    VoxelCoordinate_uint8 coord;
    VoxelType type;

    Mod() {}
    Mod(VoxelCoordinate_uint8 coord, VoxelType type)
        : coord(coord), type(type) {}
};

using ChunkIndex = uint32_t;
using ChunkModMap = std::unordered_map<VoxelCoordinate, VoxelType>;

class ModManager 
{
    public:
        ModManager(std::string regionName);

        void loadMods(const ChunkCoordinate loc);
        void applyMods(Chunk& chunk);
        void saveMods();
        void setMod(ChunkCoordinate chunkLoc, VoxelCoordinate voxLoc, VoxelType type);
        VoxelType getMod(ChunkCoordinate chunkLoc, VoxelCoordinate voxLoc);

    private:
        ChunkIndex getChunkIndex(ChunkCoordinate loc);
        void initIndexFile();

        std::string mRegionName;
        std::string mIndexPath;
        std::string mDataPath;
        std::unordered_map<ChunkCoordinate, ChunkModMap> mMods;
};


