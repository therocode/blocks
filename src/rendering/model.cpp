#include "model.hpp"

void Model::addMesh(int32_t id, const Mesh& mesh)
{
    mMeshes.emplace(id, mesh);
}

const Mesh* Model::findMesh(int32_t id) const
{
    const auto& iterator = mMeshes.find(id);

    return iterator == mMeshes.end() ? nullptr : &iterator->second;
}
