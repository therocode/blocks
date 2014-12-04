#pragma once
#include "buffer.hpp"

class Mesh
{
    public:
        Mesh(const std::vector<uint32_t>& indices);
        const Buffer& getIndexBuffer() const;
    private:
        Buffer mIndexBuffer;
};
