#pragma once

#include <cstdint>
#include <cstdio>
#include <exception>
#include <fstream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "chunk.h"

#define PR(x) std::cerr << #x << " = " << (x) << std::endl;

#define NO_CHUNK -1

extern const std::string regionDir;
extern const uint8_t regionWidth;

struct VoxelCoordinate_uint8
{
    VoxelCoordinate_uint8();
    VoxelCoordinate_uint8(VoxelCoordinate coord);

    bool operator==(const VoxelCoordinate_uint8& other) const;
    bool operator!=(const VoxelCoordinate_uint8& other) const;

    bool operator<(const VoxelCoordinate_uint8& other) const;

    uint8_t x, y, z;
};

namespace std
{
    template<>
        struct hash<VoxelCoordinate_uint8>
        {
            public:
                std::size_t operator()(VoxelCoordinate_uint8 const& coord) const 
                {
                    /*
                    size_t x = coord.x;

                                      x = 00000000 00000000 00000000 00000000 00000000 00000000 00000000 abcdefgh
                                          00000000 00000000 00000000 00000000 00000000 00000000 abcdefgh 00000000 << 8
                                          00000000 00000000 00000000 00000000 00000000 00000000 abcdefgh abcdefgh | x
                                          00000000 00000000 00000000 00000000 00000000 00000000 11110000 00001111 &
                                      x = 00000000 00000000 00000000 00000000 00000000 00000000 abcd0000 0000efgh
                                          00000000 00000000 00000000 00000000 00000000 0000abcd 00000000 efgh0000 << 4
                                          00000000 00000000 00000000 00000000 00000000 0000abcd abcd0000 efghefgh | x
                                          00000000 00000000 00000000 00000000 00000000 00001100 00110000 11000011 &
                                      x = 00000000 00000000 00000000 00000000 00000000 0000ab00 00cd0000 ef0000gh 
                                          00000000 00000000 00000000 00000000 00000000 00ab0000 cd0000ef 0000gh00 << 2
                                          00000000 00000000 00000000 00000000 00000000 00abab00 cdcd00ef ef00ghgh | x
                                          00000000 00000000 00000000 00000000 00000000 00100100 10010010 01001001 &
                                      x = 00000000 00000000 00000000 00000000 00000000 00a00b00 c00d00e0 0f00g00h
                                      */
                    size_t x = coord.x;
                    x = (x | (x << 8))  & 0x000000000000F00F;
                    x = (x | (x << 4))  & 0x00000000000C30C3;
                    x = (x | (x << 2))  & 0x0000000000249249;

                    size_t y = coord.y;
                    y = (y | (y << 8))  & 0x000000000000F00F;
                    y = (y | (y << 4))  & 0x00000000000C30C3;
                    y = (y | (y << 2))  & 0x0000000000249249;

                    size_t z = coord.z;
                    z = (z | (z << 8))  & 0x000000000000F00F;
                    z = (z | (z << 4))  & 0x00000000000C30C3;
                    z = (z | (z << 2))  & 0x0000000000249249;

                    return x | (y << 1) | (z << 2);
                }
        };
}

struct Mod
{
    VoxelCoordinate_uint8 coord;
    VoxelType type;

    Mod() {}
    Mod(VoxelCoordinate_uint8 coord, VoxelType type)
        : coord(coord), type(type) {}
};

using ChunkIndex = uint32_t;
using ChunkModMap = std::unordered_map<VoxelCoordinate_uint8, VoxelType>;
using RegionModMap = std::unordered_map<ChunkRegionCoordinate, ChunkModMap>;

struct ModManagerException : public std::exception
{
    std::string s;
    ModManagerException(std::string ss) : s(ss) {}
    ~ModManagerException() throw () {} // Updated
    const char* what() const throw() { return s.c_str(); }
};

class ModManager 
{
    public:
        ModManager(std::string regionName);

        void loadMods(const ChunkRegionCoordinate loc);
        void applyMods(Chunk& chunk);
        void saveMods();
        void setMod(ChunkRegionCoordinate chunkLoc, VoxelCoordinate_uint8 voxLoc, VoxelType type);
        void setMod(ChunkRegionCoordinate chunkLoc, VoxelCoordinate voxLoc, VoxelType type);
        VoxelType getMod(ChunkRegionCoordinate chunkLoc, VoxelCoordinate voxLoc);

    private:
        ChunkIndex getChunkIndex(ChunkRegionCoordinate loc);
        void initIndexFile();

        std::string mRegionName;
        std::string mIndexPath;
        std::string mDataPath;
        RegionModMap mMods;
};


