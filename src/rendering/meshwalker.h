#pragma once
#include "world/chunk.h"
#include "surfacemerger.h"

using RleIterator = const uint16_t*;

class MeshWalker
{
    public:
        MeshWalker();
        void setIterators(RleIterator centreSegment, RleIterator topSegment, RleIterator bottomSegment, RleIterator frontSegment, RleIterator backSegment, uint32_t y, uint32_t z);
        const std::vector<SurfaceQuad>& getTopQuads() const;
        const std::vector<SurfaceQuad>& getBottomQuads() const;
        const std::vector<SurfaceQuad>& getFrontQuads() const;
        const std::vector<SurfaceQuad>& getBackQuads() const;
        const std::vector<SurfaceQuad>& getLeftQuads() const;
        const std::vector<SurfaceQuad>& getRightQuads() const;
        void walk();
    private:
        void walkBuildTop(uint16_t targetCoord, uint16_t currentType);
        RleIterator mCentreSegment;
        RleIterator mTopSegment;
        RleIterator mBottomSegment;
        RleIterator mFrontSegment;
        RleIterator mBackSegment;

        std::vector<SurfaceQuad> mTopQuads;
        std::vector<SurfaceQuad> mBottomQuads;
        std::vector<SurfaceQuad> mFrontQuads;
        std::vector<SurfaceQuad> mBackQuads;
        std::vector<SurfaceQuad> mLeftQuads;
        std::vector<SurfaceQuad> mRigthQuads;

        uint16_t mTopWalked;
        uint16_t mBottomWalked;
        uint16_t mFrontWalked;
        uint16_t mBackWalked;

        uint32_t mY;
        uint32_t mZ;
};
