#include "meshwalker.h"
#include <iostream>

MeshWalker::MeshWalker()
{
    mCentreSegment = nullptr;
    mTopSegment = nullptr;
    mBottomSegment = nullptr;
    mFrontSegment = nullptr;
    mBackSegment = nullptr;
    mDepth = 0;
}

void MeshWalker::setIterators(RleIterator centreSegment, RleIterator topSegment, RleIterator bottomSegment, RleIterator frontSegment, RleIterator backSegment, uint32_t depth)
{
    mCentreSegment = centreSegment;
    mTopSegment = topSegment;
    mBottomSegment = bottomSegment;
    mFrontSegment = frontSegment;
    mBackSegment = backSegment;
    mDepth = depth;
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
        mTopWalked = 0;
        mBottomWalked = 0;
        mFrontWalked = 0;
        mBackWalked = 0;

        uint16_t targetCoord = 0;
        uint16_t currentType = 0;

        while(targetCoord < chunkWidth)
        {
            targetCoord += *mCentreSegment;
            mCentreSegment++;
            currentType = *mCentreSegment;
            mCentreSegment++;

            //skip if it is air
            if(currentType == 0)
                continue;

            //let others catch up and build quads
            if(mTopSegment)
                walkBuildTop(targetCoord, currentType);
        }
    }
}

void MeshWalker::walkBuildTop(uint16_t targetCoord, uint16_t targetType)
{
    uint16_t currentType = 0;
    uint16_t quadStart = 0;

    while(mTopWalked < targetCoord)
    {
        quadStart = mTopWalked;

        mTopWalked += *mTopSegment;
        mTopSegment++;
        currentType = *mTopSegment;
        mTopSegment++;

        //don't build quad if it isn't air
        if(currentType != 0)
            continue;

        std::cout << "walked and created a length of " << mTopWalked - quadStart << "\n";
        mTopQuads.push_back(SurfaceQuad(quadStart, mTopWalked, mTopWalked - quadStart, 1, mDepth + 1, targetType));
    }
}
