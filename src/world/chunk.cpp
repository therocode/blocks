#include "chunk.h"
#include <iostream>
#include <chrono>
#include <algorithm>

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

Chunk::Chunk()
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

//changing the fourth letter to b:

//(the ones in caps belong to a group that needed to change)
//AAAABBBB -> AAABBBBB = 4A4B -> 3A5B
//BBBABBBB -> BBBBBBBB = 3B1A4B -> 8B
//BBBAAAAA -> BBBBAAAA = 3B5A -> 4B4A
//AAAAAAAA -> AAABAAAA = 8A -> 3A1B4A
//CCCCaaaa -> CCCBaaaa = 4C4a -> 3C1B4a
//cacAcaca -> cacBcaca = 1c1a1c1A1c1a1c1a -> 1c1a1c1B1c1a1c1a
//baBAcaba -> baBBcaba = 1b1a1B1A1c1a1b1a -> 1b1a2B1c1a1b1a


//which groups need to change?
//-always the ones _at_ the coordinate of change
//-the neighbour groups of the coordinate which have the same type as the one you set the coordinate to

//algorithm:
//find group belonging to the target coordinate and set that to group X
//check neighbouring groups of group X. if these carries the target type, extract these as well, as groups XL and XR (X Left and X Right)
//


//16 = 3 + 6 + 5 + 2
//
//aaabbbbbbcccccdd
//              ^
//x=14
//steps = 2
//
//walked = 14
//3a6b5c2d
//      ^

//baa
//aba
//aab
//ba
//ab
//b

void Chunk::setVoxelType(uint32_t x, uint32_t y, uint32_t z, VoxelType type)
{
    if(x >= chunkWidth || y >= chunkWidth || z >= chunkWidth)
        return;

    VoxelSegmentTypeArray uncompressed = getUncompressedTypeSegment(y, z);

    uncompressed[x] = type;

    setSegmentTypeFromArray(y, z, uncompressed);
}

void Chunk::setVoxelType(const VoxelCoordinate& voxel, VoxelType type)
{
    setVoxelType(voxel.x, voxel.y, voxel.z, type);
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

VoxelSegmentTypeArray Chunk::getUncompressedTypeSegment(uint32_t y, uint32_t z) const
{
    size_t segmentIterator = mRleSegmentIndices[z + y * chunkWidth].mSegmentStart;
    size_t walked = 0;
    VoxelSegmentTypeArray result;

    while(walked < chunkWidth)
    {
        size_t thisOne = walked;
        walked += mRleSegments[segmentIterator];


        uint16_t typeToAdd = mRleSegments[segmentIterator + 1];
        for(uint32_t i = 0; i < mRleSegments[segmentIterator]; i++)
        {
            result[thisOne + i] = typeToAdd;
        }

        segmentIterator += 2;
    }
        
    return result;
}

void Chunk::setSegmentTypeFromArray(uint16_t y, uint16_t z, const VoxelSegmentTypeArray& typeArray)
{
    uint32_t zyIndex = z + y * chunkWidth;

    std::vector<uint16_t> compressed;

    uint16_t currentType = typeArray[0];
    uint16_t currentAmount = 1;
    for(uint32_t x = 1; x < chunkWidth; x++)
    {
        //check if the new voxel is still the same
        if(currentType == typeArray[x])
        {
            //keep collecting
            currentAmount++;
        }
        else
        {
            //push the finished run and reinitialise for a new one
            compressed.push_back(currentAmount);
            compressed.push_back(currentType);

            currentType = typeArray[x];
            currentAmount = 1;
        }
    }
    compressed.push_back(currentAmount);
    compressed.push_back(currentType);

    uint16_t segmentStart = mRleSegmentIndices[zyIndex].mSegmentStart;
    uint16_t newSegmentLength = compressed.size();
    uint16_t oldSegmentLength = mRleSegmentIndices[zyIndex].mSegmentSize;
    int16_t overlap = newSegmentLength - oldSegmentLength;

    mRleSegmentIndices[zyIndex].mSegmentSize = newSegmentLength;

    //let rle array overwrite current segment, leaving the overlap amount (negative or positive)
    for(uint16_t i = 0; i < newSegmentLength && i < oldSegmentLength; i++)
    {
        mRleSegments[segmentStart + i] = compressed[i];
    }


    if(overlap < 0)
    {
        //if overlap is negative, then erase the rest
        mRleSegments.erase(mRleSegments.begin() + segmentStart + newSegmentLength - overlap, mRleSegments.begin() + segmentStart + newSegmentLength);
    }
    else if(overlap > 0)
    {
        //if overlap is positive, then move the rest
        mRleSegments.insert(mRleSegments.begin() + segmentStart + newSegmentLength - overlap, compressed.begin() + oldSegmentLength, compressed.end());
    }

    if(overlap != 0)
    {
        for(uint32_t zi = z; z < chunkWidth; z++)
        {
            for(uint32_t yi = y+1; y < chunkWidth; y++)
            {
                mRleSegmentIndices[zi + yi * chunkWidth].mSegmentStart += overlap;
            }
        }
    }
    //std::cout << "size of new: " << newSegmentLength << " size of old: " << oldSegmentLength << " overlap: " << overlap << "\n";
}
