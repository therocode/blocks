#include "surfacemerger.h"
#include <algorithm>
#include <iostream>

SurfaceQuad::SurfaceQuad(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t depth, uint16_t type) :
    mX(x),
    mY(y),
    mWidth(width),
    mHeight(height),
    mDepth(depth),
    mType(type)
{
}

void SurfaceMerger::setQuads(const std::vector<SurfaceQuad>& quads)
{
    mQuads = quads;
}

void SurfaceMerger::doFirstMerge()
{
    if(mQuads.size() > 1)
    {
        std::sort(mQuads.begin(), mQuads.end(), []( const SurfaceQuad& quadA, const SurfaceQuad& quadB)
                {
                    if(quadA.mType < quadB.mType)
                        return true;
                    if(quadA.mType > quadB.mType)
                        return false;

                    if(quadA.mDepth < quadB.mDepth)
                        return true;
                    if(quadA.mDepth > quadB.mDepth)
                        return false;

                    if(quadA.mY < quadB.mY)
                        return true;
                    if(quadA.mY > quadB.mY)
                        return false;

                    if(quadA.mX < quadB.mX)
                        return true;
                    if(quadA.mX > quadB.mX)
                        return false;

                    return false;
                });

        for(uint32_t i = 0; i < mQuads.size() - 1; i++)
        {
            SurfaceQuad& quadA = mQuads[i];
            SurfaceQuad& quadB = mQuads[i + 1];

            if((quadA.mX + quadA.mWidth == quadB.mX) && 
                    (quadA.mY == quadB.mY) && 
                    (quadA.mHeight == quadB.mHeight) && 
                    (quadA.mDepth == quadB.mDepth) &&
                    (quadA.mType == quadB.mType))
            {
                quadA.mWidth += quadB.mWidth;
                mQuads.erase(mQuads.begin() + i + 1);
                i--;
            }
        }
    }
}

void SurfaceMerger::doSecondMerge()
{
    if(mQuads.size() > 1)
    {
        //std::cout << "preparing merge and size is " << mQuads.size() << "!\n";

        std::sort(mQuads.begin(), mQuads.end(), []( const SurfaceQuad& quadA, const SurfaceQuad& quadB)
                {
                    if(quadA.mType < quadB.mType)
                        return true;
                    if(quadA.mType > quadB.mType)
                        return false;

                    if(quadA.mDepth < quadB.mDepth)
                        return true;
                    if(quadA.mDepth > quadB.mDepth)
                        return false;

                    if(quadA.mX < quadB.mX)
                        return true;
                    if(quadA.mX > quadB.mX)
                        return false;

                    if(quadA.mY < quadB.mY)
                        return true;
                    if(quadA.mY > quadB.mY)
                        return false;

                    return false;
                });

        for(uint32_t i = 0; i < mQuads.size() - 1; i++)
        {
            SurfaceQuad& quadA = mQuads[i];
            SurfaceQuad& quadB = mQuads[i + 1];
            //std::cout << "hej total amount of quads is " << mQuads.size() << "\n";

            if((quadA.mY + quadA.mHeight == quadB.mY) && 
                    (quadA.mX == quadB.mX) && 
                    (quadA.mWidth == quadB.mWidth) && 
                    (quadA.mDepth == quadB.mDepth) &&
                    (quadA.mType == quadB.mType))
            {
                //std::cout << "merging quad(" << quadA.mX << "," << quadA.mY << "," << quadA.mHeight << ") with quad(" << quadB.mX << "," << quadB.mY << "," << quadB.mHeight << "\n";
                quadA.mHeight += quadB.mHeight;
                mQuads.erase(mQuads.begin() + i + 1);
                i--;
            }
        }
    }
}

const std::vector<SurfaceQuad>& SurfaceMerger::getQuads()
{
    return mQuads;
}
