#include "mesh.hpp"

Mesh::Mesh(const std::vector<uint32_t>& indices)
{
    mIndices = indices;
}

const std::vector<uint32_t>& Mesh::getIndices() const
{
    return mIndices;
}
