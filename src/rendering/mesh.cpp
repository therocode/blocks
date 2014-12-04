#include "mesh.hpp"

Mesh::Mesh(const std::vector<uint32_t>& positions) :
    mIndexBuffer(Buffer::ELEMENT_ARRAY_BUFFER)
{
    mIndexBuffer.setData(positions);
}

const Buffer& Mesh::getIndexBuffer() const
{
    return mIndexBuffer;
}
