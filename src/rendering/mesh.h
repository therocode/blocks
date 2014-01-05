#pragma once

struct Mesh
{
    private:
        std::unique_ptr<float[]> mVertices;
        std::unique_ptr<float[]> mColour;
        std::unique_ptr<uint32_t[]> mIndices;
};
