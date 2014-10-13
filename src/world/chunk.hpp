#pragma once
#include <cstdint>
#include <functional>
#include <array>
#include <vector>
#include <unordered_map>
#include "worlddefines.hpp"
#include "../utilities/glmhash.hpp"

struct RleSegmentInfo
{
    bool operator==(const RleSegmentInfo& other) const;
    uint32_t mSegmentStart;
    uint32_t mSegmentSize;
};

using VoxelTypeArray = std::array<VoxelType, voxelAmount>;
using VoxelSegmentTypeArray = std::array<VoxelType, chunkWidth>;
using RleIndexArray = std::array<RleSegmentInfo, chunkWidthPow2>;
using RleSegmentArray = std::vector<uint16_t>;

struct VoxelTypeData
{
    VoxelTypeData(const RleIndexArray& rleSegmentIndices, const RleSegmentArray& rleSegments);
    const RleIndexArray& mRleSegmentIndices;
    const RleSegmentArray& mRleSegments;    
};

enum Solidity {EMPTY, SOLID, INBETWEEN};

class Chunk
{
    public:
        Chunk();
        Chunk(const VoxelTypeArray& types);
        Chunk(const RleIndexArray& indices, const RleSegmentArray& rleData);
        bool operator==(const Chunk& other) const;
        void setVoxelType(const ChunkVoxelCoord& voxel, VoxelType type);
        void setVoxelData(const VoxelTypeArray& types);
        VoxelType getVoxelType(const ChunkVoxelCoord& voxel) const;
        VoxelTypeData getVoxelTypeData() const;
        VoxelTypeArray getFlatVoxelTypeData() const;
		Solidity getSolidity() const;
		Solidity getSideSolidity(CubeFace side) const;
    private:
        VoxelSegmentTypeArray getUncompressedTypeSegment(uint32_t y, uint32_t z) const;
        void setSegmentTypeFromArray(uint16_t y, uint16_t z, const VoxelSegmentTypeArray& typeArray);
		void solidityCheck();
		void sideSolidityCheck();
		Solidity mSolidity;
		Solidity mSideSolidities[6];

        RleIndexArray mRleSegmentIndices;
        RleSegmentArray mRleSegments;
};

using ChunkMap = std::unordered_map<ChunkCoord, Chunk>;
using ChunkReferenceMap = std::unordered_map<ChunkCoord, const Chunk&>;
