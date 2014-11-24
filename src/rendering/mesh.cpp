#include "mesh.hpp"

Mesh::Mesh(const std::vector<float>& positions)
{
    mPositionBuffer.setData(positions);
}

const Buffer& Mesh::getPositionBuffer() const
{
    return mPositionBuffer;
}
