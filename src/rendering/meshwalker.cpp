#include "meshwalker.h"
#include <iostream>

void MeshWalker::NeighbourWalker::setIterator(RleIterator iterator)
{
    mSegment = iterator;
}

void MeshWalker::NeighbourWalker::walk()
{
    airField.reset();
    uint16_t walked = 0;

    if(mSegment == nullptr)
    {
        airField.set();
        return;
    }

    while(walked < chunkWidth)
    {
        uint16_t airStart = walked;
        walked += *mSegment;
        mSegment++;
        uint16_t type = *mSegment;
        mSegment++;

        if(type == 0)
        {
            uint16_t airStop = walked;

            for(uint16_t i = airStart; i < airStop; i++)
            {
                airField.set(i);
            }
        }
    }
}

std::vector<uint16_t> MeshWalker::NeighbourWalker::requestQuadCoords(uint16_t start, uint16_t stop)
{
    std::vector<uint16_t> resultingQuads;
    quadField.reset();

    for(uint16_t i = start; i < stop; i++)
    {
        quadField.set(i);
    }

    quadField = quadField & airField;

    bool inQuad = false;
    for(uint16_t i = 0; i < chunkWidth; i++)
    {
        if(quadField[i] && !inQuad)
        {
            resultingQuads.push_back(i);
            inQuad = true;
        }
        else if(!quadField[i] && inQuad)
        {
            resultingQuads.push_back(i);
            inQuad = false;
        }
    }

    if(inQuad)
        resultingQuads.push_back(chunkWidth);

    return resultingQuads;
}

MeshWalker::MeshWalker()
{
    mY = 0;
    mZ = 0;
}

void MeshWalker::setIterators(RleIterator centreSegment, RleIterator topSegment, RleIterator bottomSegment, RleIterator frontSegment, RleIterator backSegment, uint32_t y, uint32_t z)
{
    mCentreSegment = centreSegment;
    mTop.setIterator(topSegment);
    mBottom.setIterator(bottomSegment);
    mFront.setIterator(frontSegment);
    mBack.setIterator(backSegment);
    mY = y;
    mZ = z;
}
        
const std::vector<SurfaceQuad>& MeshWalker::getTopQuads() const
{
    return mTopQuads;
}

const std::vector<SurfaceQuad>& MeshWalker::getBottomQuads() const
{
    return mBottomQuads;
}

const std::vector<SurfaceQuad>& MeshWalker::getFrontQuads() const
{
    return mFrontQuads;
}

const std::vector<SurfaceQuad>& MeshWalker::getBackQuads() const
{
    return mBackQuads;
}

const std::vector<SurfaceQuad>& MeshWalker::getLeftQuads() const
{
    return mLeftQuads;
}

const std::vector<SurfaceQuad>& MeshWalker::getRightQuads() const
{
    return mRigthQuads;
}

void MeshWalker::walk()
{
    if(mCentreSegment)
    {
        mTop.walk();

        mTopWalked = 0;
        mBottomWalked = 0;
        mFrontWalked = 0;
        mBackWalked = 0;

        uint16_t targetCoord = 0;
        uint16_t currentType = 0;
        uint16_t quadStart = 0;

        while(targetCoord < chunkWidth)
        {
            quadStart = targetCoord;
            targetCoord += *mCentreSegment;
            mCentreSegment++;
            currentType = *mCentreSegment;
            mCentreSegment++;

            //skip if it is air
            if(currentType == 0)
                continue;

            std::vector<uint16_t> topQuads = mTop.requestQuadCoords(quadStart, targetCoord);

            for(uint32_t i = 0; i < topQuads.size(); i += 2)
            {
                mTopQuads.push_back(SurfaceQuad(topQuads[i], mZ, topQuads[i + 1] - topQuads[i], 1, mY, currentType));
            }
        }
    }
}
