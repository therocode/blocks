#pragma once
#include <stdint.h>
#include <functional>
#include <unordered_map>
#include "worldstd.h"

class Chunk
{
    public:
        Chunk();
        Chunk(const ChunkCoordinate& loc);
        Chunk(const ChunkCoordinate& loc, const VoxelTypeArray& types);
        Chunk(const ChunkCoordinate& loc, const RleIndexArray& indices, const RleSegmentArray& rleData);
        void setVoxelType(uint32_t x, uint32_t y, uint32_t z, VoxelType type);
        void setVoxelType(const VoxelChunkCoordinate& voxel, VoxelType type);
        void setVoxelData(const VoxelTypeArray& types);
        VoxelType getVoxelType(uint32_t x, uint32_t y, uint32_t z) const;
        VoxelType getVoxelType(const VoxelChunkCoordinate& voxel) const;
        VoxelTypeData getVoxelTypeData() const;
        uint32_t getWidth() const;
        const ChunkCoordinate& getLocation() const; //should probably be gone
    private:
        VoxelSegmentTypeArray getUncompressedTypeSegment(uint32_t y, uint32_t z) const;
        void setSegmentTypeFromArray(uint16_t y, uint16_t z, const VoxelSegmentTypeArray& typeArray);
        ChunkCoordinate mLocation;

        RleIndexArray mRleSegmentIndices;
        RleSegmentArray mRleSegments;

        static uint32_t totalTime;
        static uint32_t totalSize;
        static uint32_t timesGenerated;
};

using ChunkMap = std::unordered_map<ChunkCoordinate, Chunk>;
using ChunkReferenceMap = std::unordered_map<ChunkCoordinate, const Chunk&>;

namespace std
{
    template<>
        struct hash<ChunkCoordinate>
        {
            public:
                std::size_t operator()(ChunkCoordinate const& coord) const 
                {
                    /*
                    size_t x = coord.x;

                                      x = 00000000 00000000 00000000 00000000 01234567 89abcdef ghijklmn opqrstuv
                                          01234567 89abcdef ghijklmn opqrstuv 00000000 00000000 00000000 00000000 << 32
                                          01234567 89abcdef ghijklmn opqrstuv 01234567 89abcdef ghijklmn opqrstuv | x
                                          11111111 11111111 00000000 00000000 00000000 00000000 11111111 11111111 &
                                      x = 01234567 89abcdef 00000000 00000000 00000000 00000000 ghijklmn opqrstuv
                                          00000000 00000000 00000000 00000000 ghijklmn opqrstuv 00000000 00000000 << 16
                                          01234567 89abcdef 00000000 00000000 ghijklmn opqrstuv ghijklmn opqrstuv | x
                                          00000000 11111111 00000000 00000000 11111111 00000000 00000000 11111111 &
                                      x = 00000000 89abcdef 00000000 00000000 ghijklmn 00000000 00000000 opqrstuv 
                                          89abcdef 00000000 00000000 ghijklmn 00000000 00000000 opqrstuv 00000000 << 8
                                          89abcdef 89abcdef 00000000 ghijklmn ghijklmn 00000000 opqrstuv opqrstuv | x
                                          11110000 00001111 00000000 11110000 00001111 00000000 11110000 00001111 &
                                      x = 89ab0000 0000cdef 00000000 ghij0000 0000klmn 00000000 opqr0000 0000stuv
                                          00000000 cdef0000 0000ghij 00000000 klmn0000 0000opqr 00000000 stuv0000 << 4
                                          89ab0000 cdefcdef 0000ghij ghij0000 klmnklmn 0000opqr opqr0000 stuvstuv | x
                                          00110000 11000011 00001100 00110000 11000011 00001100 00110000 11000011 &
                                      x = 00ab0000 cd0000ef 0000gh00 00ij0000 kl0000mn 0000op00 00qr0000 st0000uv
                                          ab0000cd 0000ef00 00gh0000 ij0000kl 0000mn00 00op0000 qr0000st 0000uv00 << 2
                                          abab00cd cd00efef 00ghgh00 ijij00kl kl00mnmn 00opop00 qrqr00st st00uvuv | x
                                          10010010 01001001 00100100 10010010 01001001 00100100 10010010 01001001 &
                                      x = a00b00c0 0d00e00f 00g00h00 i00j00k0 0l00m00n 00o00p00 q00r00s0 0t00u00v
                                      */
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

namespace std
{
    template<>
        struct hash<ChunkRegionCoordinate>
        {
            public:
                std::size_t operator()(ChunkRegionCoordinate const& coord) const 
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
