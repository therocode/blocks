#pragma once
#include <cstdint>
#include <functional>
#include <unordered_map>
#include "worldstd.hpp"
#include "../utilities/glmhash.hpp"

enum Solidity {EMPTY, SOLID, INBETWEEN};

class Chunk
{
    public:
        Chunk();
        Chunk(const ChunkCoord& loc);
        Chunk(const ChunkCoord& loc, const VoxelTypeArray& types);
        Chunk(const ChunkCoord& loc, const RleIndexArray& indices, const RleSegmentArray& rleData);
        void setVoxelType(const ChunkVoxelCoord& voxel, VoxelType type);
        void setVoxelData(const VoxelTypeArray& types);
        VoxelType getVoxelType(const ChunkVoxelCoord& voxel) const;
        VoxelTypeData getVoxelTypeData() const;
        VoxelTypeArray getFlatVoxelTypeData() const;
        uint32_t getWidth() const;
        const ChunkCoord& getLocation() const; //should probably be gone
		enum ChunkSide {TOP, LEFT, RIGHT, BOTTOM, FRONT, BACK};
		Solidity getSolidity();
		Solidity getSideSolidity(ChunkSide side);
    private:
        VoxelSegmentTypeArray getUncompressedTypeSegment(uint32_t y, uint32_t z) const;
        void setSegmentTypeFromArray(uint16_t y, uint16_t z, const VoxelSegmentTypeArray& typeArray);
		void solidityCheck();
		void sideSolidityCheck();
        ChunkCoord mLocation;
		Solidity mSolidity;
		Solidity mSideSolidities[6];

        RleIndexArray mRleSegmentIndices;
        RleSegmentArray mRleSegments;

        static uint32_t totalTime;
        static uint32_t totalSize;
        static uint32_t timesGenerated;
};

using ChunkMap = std::unordered_map<ChunkCoord, Chunk>;
using ChunkReferenceMap = std::unordered_map<ChunkCoord, const Chunk&>;
