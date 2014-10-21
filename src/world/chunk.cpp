#include "chunk.hpp"
#include <algorithm>
#include <fea/assert.hpp>

bool RleSegmentInfo::operator==(const RleSegmentInfo& other) const
{
    return mSegmentStart == other.mSegmentStart && mSegmentSize == other.mSegmentSize;
}

VoxelTypeData::VoxelTypeData(const RleIndexArray& rleSegmentIndices, const RleSegmentArray& rleSegments) : mRleSegmentIndices(rleSegmentIndices), mRleSegments(rleSegments)
{

}

Chunk::Chunk()
{
    VoxelTypeArray types;
    types.fill(0);
    setVoxelData(types);
}

Chunk::Chunk(const VoxelTypeArray& types)
{
    setVoxelData(types);
}

Chunk::Chunk(const RleIndexArray& indices, const RleSegmentArray& rleData) : mRleSegmentIndices(indices), mRleSegments(rleData)
{
}

bool Chunk::operator==(const Chunk& other) const
{
    return mRleSegmentIndices == other.mRleSegmentIndices && mRleSegments == other.mRleSegments;
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

void Chunk::setVoxelType(const ChunkVoxelCoord& voxel, VoxelType type)
{
	FEA_ASSERT(voxel.x < chunkWidth && voxel.y < chunkWidth && voxel.z < chunkWidth, "Setting voxel " + glm::to_string((glm::ivec3) voxel) + " which is out of bounds of the chunk");

    VoxelSegmentTypeArray uncompressed = getUncompressedTypeSegment(voxel.y, voxel.z);

    uncompressed[voxel.x] = type;

    setSegmentTypeFromArray(voxel.y, voxel.z, uncompressed);

	if(mSolidity != INBETWEEN)
	{
		Solidity s = type ? SOLID : EMPTY;
		mSolidity = (mSolidity == s) ? mSolidity : INBETWEEN;
	}
	else
	{
		solidityCheck();
	}
	
	if(mSolidity != INBETWEEN)
	{
		for(auto &v : mSideSolidities)
			v = mSolidity;
	}
	else
	{
		sideSolidityCheck();
	}
}

void Chunk::setVoxelData(const VoxelTypeArray& types)
{
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

            zyIndex = z * chunkWidth + y * chunkWidthPow2;
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

	solidityCheck();
	if(mSolidity == INBETWEEN)
	{
		sideSolidityCheck();
	}
	else
	{
		for(auto &v : mSideSolidities)
			v = mSolidity;
	}
}

//these should be optimised in the future using binary trees
VoxelType Chunk::getVoxelType(const ChunkVoxelCoord& voxel) const
{
	FEA_ASSERT(voxel.x < chunkWidth && voxel.y < chunkWidth && voxel.z < chunkWidth, "Accessing voxel " + glm::to_string((glm::ivec3)voxel) + " which is out of bounds of the chunk");
    size_t segmentIterator = mRleSegmentIndices[voxel.z + voxel.y * chunkWidth].mSegmentStart;
    size_t walked = 0;

    while(walked < voxel.x)
    {
        walked += mRleSegments[segmentIterator];

        if(walked <= voxel.x)
            segmentIterator += 2;
    }
    
    return mRleSegments[segmentIterator + 1];
}

VoxelTypeData Chunk::getVoxelTypeData() const
{
    return VoxelTypeData(mRleSegmentIndices, mRleSegments);
}

VoxelTypeArray Chunk::getFlatVoxelTypeData() const
{
    VoxelTypeArray typeData;
    size_t iterator = 0;

    for(uint32_t y = 0; y < chunkWidth; y++)
    {
        for(uint32_t z = 0; z < chunkWidth; z++)
        {
            VoxelSegmentTypeArray segment = getUncompressedTypeSegment(y, z);

            std::copy(segment.begin(), segment.end(), typeData.begin() + iterator);
            iterator += segment.size();
        }
    }
    return typeData;
}

Solidity Chunk::getSolidity() const
{
	return mSolidity;
}

Solidity Chunk::getSideSolidity(CubeFace side) const
{
	return mSideSolidities[side];
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

    int16_t segmentStart = mRleSegmentIndices[zyIndex].mSegmentStart;
    int16_t newSegmentLength = compressed.size();
    int16_t oldSegmentLength = mRleSegmentIndices[zyIndex].mSegmentSize;
    int16_t overlap = newSegmentLength - oldSegmentLength;

    mRleSegmentIndices[zyIndex].mSegmentSize = newSegmentLength;

    //let rle array overwrite current segment, leaving the overlap amount (negative or positive)
    for(uint16_t i = 0; i < newSegmentLength && i < oldSegmentLength; i++)
    {
        mRleSegments[segmentStart + i] = compressed[i];
    }


    if(overlap < 0)
    {
        mRleSegments.erase(mRleSegments.begin() + segmentStart + newSegmentLength, mRleSegments.begin() + segmentStart + oldSegmentLength);
    }
    else if(overlap > 0)
    {
        mRleSegments.insert(mRleSegments.begin() + segmentStart + oldSegmentLength, compressed.begin() + oldSegmentLength, compressed.end());
    }

    if(overlap != 0)
    {
        for(uint32_t zi = 0; zi < chunkWidth; zi++)
        {
            for(uint32_t yi = 0; yi < chunkWidth; yi++)
            {
                if(mRleSegmentIndices[zyIndex].mSegmentStart >= mRleSegmentIndices[zi + yi * chunkWidth].mSegmentStart)
                    continue;

                mRleSegmentIndices[zi + yi * chunkWidth].mSegmentStart += overlap;
            }
        }
    }
}

void Chunk::solidityCheck()
{
	VoxelTypeArray types = getFlatVoxelTypeData();
	
	mSolidity = types[0] ? SOLID : EMPTY;
	for(auto v : types)
	{
		Solidity s = v ? SOLID : EMPTY;
		if(mSolidity != s)
		{
			mSolidity = INBETWEEN;
			break;
		}
	}
}

void Chunk::sideSolidityCheck()
{
	VoxelTypeArray types = getFlatVoxelTypeData();

	mSideSolidities[CUBE_BOTTOM] = types[0] ? SOLID : EMPTY;
	for(int x=0; x<chunkWidthPow2; ++x)
	{
		Solidity s = types[x] ? SOLID : EMPTY;
		if(mSideSolidities[CUBE_BOTTOM] != s)
		{
			mSideSolidities[CUBE_BOTTOM] = INBETWEEN;
			break;
		}
	}

	mSideSolidities[CUBE_TOP] = types[chunkWidthPow3 - chunkWidthPow2] ? SOLID : EMPTY;
	for(int x=0; x<chunkWidthPow2; ++x)
	{
		Solidity s = types[x + (chunkWidthPow3 - chunkWidthPow2)] ? SOLID : EMPTY;
		if(mSideSolidities[CUBE_TOP] != s)
		{
			mSideSolidities[CUBE_TOP] = INBETWEEN;
			break;
		}
	}
	
	mSideSolidities[CUBE_FRONT] = types[0] ? SOLID : EMPTY;
	for(int y=0; y<chunkWidth; ++y)
	{
		for(int x=0; x<chunkWidth; ++x)
		{
			Solidity s = types[x + y*chunkWidthPow2] ? SOLID : EMPTY;
			if(mSideSolidities[CUBE_FRONT] != s)
			{
				mSideSolidities[CUBE_FRONT] = INBETWEEN;
				break;
			}
		}
	}
	
	mSideSolidities[CUBE_BACK] = types[chunkWidthPow2 - chunkWidth] ? SOLID : EMPTY;
	for(int y=0; y<chunkWidth; ++y)
	{
		for(int x=0; x<chunkWidth; ++x)
		{
			Solidity s = types[x + (chunkWidthPow2 - chunkWidth) + y*chunkWidthPow2] ? SOLID : EMPTY;
			if(mSideSolidities[CUBE_BACK] != s)
			{
				mSideSolidities[CUBE_BACK] = INBETWEEN;
				break;
			}
		}
	}
	
	mSideSolidities[CUBE_LEFT] = types[0] ? SOLID : EMPTY;
	for(int y=0; y<chunkWidth; ++y)
	{
		for(int z=0; z<chunkWidth; ++z)
		{
			Solidity s = types[z * chunkWidth + y * chunkWidthPow2] ? SOLID : EMPTY;
			if(mSideSolidities[CUBE_LEFT] != s)
			{
				mSideSolidities[CUBE_LEFT] = INBETWEEN;
				break;
			}
		}
	}
	
	mSideSolidities[CUBE_RIGHT] = types[chunkWidth - 1] ? SOLID : EMPTY;
	for(int y=0; y<chunkWidth; ++y)
	{
		for(int z=0; z<chunkWidth; ++z)
		{
			Solidity s = types[(chunkWidth - 1) + z * chunkWidth + y * chunkWidthPow2] ? SOLID : EMPTY;
			if(mSideSolidities[CUBE_RIGHT] != s)
			{
				mSideSolidities[CUBE_RIGHT] = INBETWEEN;
				break;
			}
		}
	}
}
