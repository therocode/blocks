#include "chunk.h"
#include <iostream>
#include <chrono>

uint32_t Chunk::totalTime = 0;
uint32_t Chunk::totalSize = 0;
uint32_t Chunk::timesGenerated = 0;

ChunkCoordinate worldToChunk(float x, float y, float z)
{
	int xNegative = (x < 0.0f)?1:0;
    int yNegative = (y < 0.0f)?1:0;
    int zNegative = (z < 0.0f)?1:0;

    return ChunkCoordinate(	((int)(x) / chunkWidth) - xNegative, 
							((int)(y) / chunkWidth) - yNegative, 
							((int)(z) / chunkWidth) - zNegative);
}

ChunkCoordinate worldToChunk(const glm::vec3& position)
{
    return worldToChunk(position.x, position.y, position.z);
}

VoxelWorldCoordinate worldToVoxel(float x, float y, float z)
{
    return VoxelWorldCoordinate((int)x - (x<0),
								(int)y - (y<0),
								(int)z - (z<0));
}

VoxelWorldCoordinate worldToVoxel(const glm::vec3& position)
{
    return worldToVoxel(position.x, position.y, position.z);
}

VoxelCoordinate worldToChunkVoxel(const glm::vec3& position)
{
	return worldToChunkVoxel(position.x, position.y, position.z);
}

VoxelCoordinate worldToChunkVoxel(float x, float y, float z)
{
	int xNegative = (int)(x - chunkWidth) / chunkWidth;
	int yNegative = (int)(y - chunkWidth) / chunkWidth;
	int zNegative = (int)(z - chunkWidth) / chunkWidth;
	
	if(xNegative < 0) x += (-xNegative + 1) * chunkWidth;
	if(yNegative < 0) y += (-yNegative + 1) * chunkWidth;
	if(zNegative < 0) z += (-zNegative + 1) * chunkWidth;

    return VoxelCoordinate((((int)x) % chunkWidth),
                           (((int)y) % chunkWidth),
                           (((int)z) % chunkWidth));
}



VoxelTypeData::VoxelTypeData(const RleIndexArray& rleSegmentIndices, const RleSegmentArray& rleSegments) : mRleSegmentIndices(rleSegmentIndices), mRleSegments(rleSegments)
{

}

Chunk::Chunk(const ChunkCoordinate& loc) : mLocation(loc)
{
}

Chunk::Chunk(const ChunkCoordinate& loc, const VoxelTypeArray& types) : mLocation(loc)
{
    setVoxelData(types);
}

Chunk::Chunk(const ChunkCoordinate& loc, const RleIndexArray& indices, const RleSegmentArray& rleData) : mLocation(loc), mRleSegmentIndices(indices), mRleSegments(rleData)
{
}

void Chunk::setVoxelType(uint32_t x, uint32_t y, uint32_t z, VoxelType type)
{
    //voxelTypes[x + z * chunkWidth + y * chunkWidthx2] = type;
}

void Chunk::setVoxelType(const VoxelCoordinate& voxel, VoxelType type)
{
    //setVoxelType(voxel.x, voxel.y, voxel.z, type);
}

void Chunk::setVoxelData(const VoxelTypeArray& types)
{
    using namespace std::chrono;

    //std::cout << "size before compression: " << sizeof(VoxelTypeArray) << "\n";
    high_resolution_clock::time_point now = high_resolution_clock::now();

    mRleSegmentIndices.fill(RleSegmentInfo({0, 0}));
    mRleSegments.clear();

    uint16_t currentType;
    uint16_t currentAmount;
    size_t zyIndex = 0;
    for(uint32_t z = 0; z < chunkWidth; z++)
    {
        for(uint32_t y = 0; y < chunkWidth; y++)
        {
            //store the start of this whole segment
            size_t segmentIndex = z + y * chunkWidth;
            mRleSegmentIndices[segmentIndex].mSegmentStart = mRleSegments.size();

            zyIndex = z * chunkWidth + y * chunkWidthx2;
            currentType = types[zyIndex];
            currentAmount = 1;
            for(uint32_t x = 1; x < chunkWidth; x++)
            {
                //check if the new voxel is still the same
                if(currentType == types[zyIndex + x])
                {
                    //keep collecting
                    currentAmount++;
                }
                else
                {
                    //push the finished run and reinitialise for a new one
                    mRleSegments.push_back(currentAmount);
                    mRleSegments.push_back(currentType);

                    currentType = types[zyIndex + x];
                    currentAmount = 1;
                }
            }

            //if it reached the end of the segment, it should add what it collected so far
            mRleSegments.push_back(currentAmount);
            mRleSegments.push_back(currentType);

            mRleSegmentIndices[segmentIndex].mSegmentSize = mRleSegments.size() - mRleSegmentIndices[segmentIndex].mSegmentStart;
        }
    }

    high_resolution_clock::time_point then = high_resolution_clock::now();

    totalTime += duration_cast<microseconds>(then - now).count();
    timesGenerated++;
    totalSize += mRleSegments.size() * sizeof(uint16_t);

    //std::cout << "size after compression: " << mRleSegments.size() * sizeof(uint16_t) << "\n";
    //std::cout << "the compression process took " << duration_cast<microseconds>(then - now).count() << " microseconds\n";
    //std::cout << "average compression is " << totalTime / timesGenerated << " microseconds and average size is " << totalSize / timesGenerated << "\n\n";
}

//these should be optimised in the future using binary trees
VoxelType Chunk::getVoxelType(uint32_t x, uint32_t y, uint32_t z) const
{
	if(x >= chunkWidth || y >= chunkWidth || z >= chunkWidth)return 0;
    size_t segmentIterator = mRleSegmentIndices[z + y * chunkWidth].mSegmentStart;
    size_t walked = 0;

    while(walked < x)
    {
        walked += mRleSegments[segmentIterator];

        if(walked <= x)
            segmentIterator += 2;
    }
    
    return mRleSegments[segmentIterator + 1];
}

VoxelType Chunk::getVoxelType(const VoxelCoordinate& voxel) const
{
    return getVoxelType(voxel.x, voxel.y, voxel.z);
}

VoxelTypeData Chunk::getVoxelTypeData() const
{
    return VoxelTypeData(mRleSegmentIndices, mRleSegments);
}

uint32_t Chunk::getWidth() const
{
    return chunkWidth;
}

const ChunkCoordinate& Chunk::getLocation() const
{
    return mLocation;
}
