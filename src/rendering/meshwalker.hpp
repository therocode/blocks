#pragma once
#include "world/chunk.hpp"
#include "surfacemerger.hpp"
#include <bitset>

using RleIterator = const uint16_t*;

class MeshWalker
{
    class NeighbourWalker
    {
        public:
            void setIterator(RleIterator iterator);
            void walk();
            const std::vector<uint16_t>& requestQuadCoords(uint16_t start, uint16_t stop);
        private:
            RleIterator mSegment;
            std::bitset<chunkWidth> quadField;
            std::bitset<chunkWidth> airField;
            std::vector<uint16_t> resultingQuads;
    };

    public:
        MeshWalker();
        void setIterators(RleIterator centreSegment, RleIterator topSegment, RleIterator bottomSegment, RleIterator frontSegment, RleIterator backSegment, uint32_t y, uint32_t z);
        void setLeftType(uint16_t type);
        void setRightType(uint16_t type);
        const std::vector<SurfaceQuad>& getTopQuads() const;
        const std::vector<SurfaceQuad>& getBottomQuads() const;
        const std::vector<SurfaceQuad>& getFrontQuads() const;
        const std::vector<SurfaceQuad>& getBackQuads() const;
        const std::vector<SurfaceQuad>& getLeftQuads() const;
        const std::vector<SurfaceQuad>& getRightQuads() const;
        void walk();
    private:
        void walkBuildTop(uint16_t targetCoord, uint16_t currentType, uint16_t earliestStart);
        RleIterator mCentreSegment;
        NeighbourWalker mTop;
        NeighbourWalker mBottom;
        NeighbourWalker mFront;
        NeighbourWalker mBack;

        std::vector<SurfaceQuad> mTopQuads;
        std::vector<SurfaceQuad> mBottomQuads;
        std::vector<SurfaceQuad> mFrontQuads;
        std::vector<SurfaceQuad> mBackQuads;
        std::vector<SurfaceQuad> mLeftQuads;
        std::vector<SurfaceQuad> mRightQuads;

        uint32_t mY;
        uint32_t mZ;

        uint16_t mLeftType;
        uint16_t mRightType;
};
