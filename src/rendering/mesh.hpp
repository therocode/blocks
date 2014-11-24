#pragma once
#include "buffer.hpp"

class Mesh
{
    public:
        Mesh(const std::vector<float>& positions);
        const Buffer& getPositionBuffer() const;
    private:
        Buffer mPositionBuffer;
};
