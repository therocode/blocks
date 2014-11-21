#pragma once
#include "buffer.hpp"

class Mesh
{
    public:
        const Buffer& getPositionBuffer() const;
    private:
        Buffer mPositionBuffer;
};
