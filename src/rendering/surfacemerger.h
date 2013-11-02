#pragma once
#include <stdint.h>
#include <vector>

class SurfaceMerger
{
    enum Orientation { UP, DOWN, LEFT, RIGHT, FRONT, BACK };

    struct SurfaceQuad
    {
        uint32_t mX;
        uint32_t mY;
        uint32_t mWidth;
        uint32_t mHeight;

        uint32_t mDepth;
    };

    public:
        void doFirstMerge();
        void doSecondMerge();
    private:
        Orientation mOrientation;
        std::vector<SurfaceQuad> mQuads;
};
