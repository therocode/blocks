#include <fea/assert.hpp>
#include "model.hpp"

void Model::addVertexArray(int32_t type, std::vector<float> vertices)
{
    FEA_ASSERT(type == POSITIONS || type == NORMALS || type == TEXCOORDS, "invaltype vertex array type given");
    FEA_ASSERT(mVertexArrays.count(type) == 0, "buffer of that type already added");

    mVertexArrays.emplace(type, std::unique_ptr<Buffer>(new Buffer(vertices)));
}

const Buffer* Model::findVertexArray(int32_t type) const
{
    const auto& iterator = mVertexArrays.find(type);

    return iterator == mVertexArrays.end() ? nullptr : iterator->second.get();
}

void Model::addMesh(int32_t id, std::unique_ptr<Mesh> mesh)
{
    mMeshes.emplace(id, std::move(mesh));
}

const Mesh* Model::findMesh(int32_t id) const
{
    const auto& iterator = mMeshes.find(id);

    return iterator == mMeshes.end() ? nullptr : iterator->second.get();
}

const std::unordered_map<int32_t, std::unique_ptr<Mesh>>& Model::getMeshes() const
{
    return mMeshes;
}
