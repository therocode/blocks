#pragma once
#include <stdint.h>
#include <vector>

struct SurfaceQuad
{
    SurfaceQuad(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t depth, uint16_t type);
    uint32_t mX;
    uint32_t mY;
    uint32_t mWidth;
    uint32_t mHeight;

    uint32_t mDepth;
    uint16_t mType;
};

class SurfaceMerger
{
    enum Orientation { UP, DOWN, LEFT, RIGHT, FRONT, BACK };

    public:
        void setQuads(const std::vector<SurfaceQuad>& quads);
        void doFirstMerge();
        void doSecondMerge();
        const std::vector<SurfaceQuad>& getQuads();
    private:
        Orientation mOrientation;
        std::vector<SurfaceQuad> mQuads;
};
