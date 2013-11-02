#include "surfacemerger.h"
#include <algorithm>

void SurfaceMerger::doFirstMerge()
{
    std::sort(mQuads.begin(), mQuads.end(), []( const SurfaceQuad& quadA, const SurfaceQuad& quadB)
    {
        if(quadA.mDepth < quadB.mDepth)
            return true;
        if(quadA.mY < quadB.mY)
            return true;
        if(quadA.mX < quadB.mX)
            return true;

        return false;
    });

    for(uint32_t i = 0; i < mQuads.size() - 1; i++)
    {
        SurfaceQuad& quadA = mQuads[i];
        SurfaceQuad& quadB = mQuads[i + 1];

        if((quadA.mX + quadA.mWidth == quadB.mX) && 
           (quadA.mY == quadB.mY) && 
           (quadA.mDepth == quadB.mDepth))
        {
            quadA.mWidth += quadB.mWidth;
            mQuads.erase(mQuads.begin() + i + 1);
            i--;
        }
    }
}

void SurfaceMerger::doSecondMerge()
{
    std::sort(mQuads.begin(), mQuads.end(), []( const SurfaceQuad& quadA, const SurfaceQuad& quadB)
    {
        if(quadA.mDepth < quadB.mDepth)
            return true;
        if(quadA.mX < quadB.mX)
            return true;
        if(quadA.mY < quadB.mY)
            return true;

        return false;
    });

    for(uint32_t i = 0; i < mQuads.size() - 1; i++)
    {
        SurfaceQuad& quadA = mQuads[i];
        SurfaceQuad& quadB = mQuads[i + 1];

        if((quadA.mY + quadA.mHeight == quadB.mY) && 
           (quadA.mX == quadB.mX) && 
           (quadA.mDepth == quadB.mDepth))
        {
            quadA.mHeight += quadB.mHeight;
            mQuads.erase(mQuads.begin() + i + 1);
            i--;
        }
    }
}
