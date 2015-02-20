#include <fea/assert.hpp>
#include "model.hpp"
#include "modelattribute.hpp"

void Model::addVertexArray(ModelAttribute type, const std::vector<float>& vertices)
{
    FEA_ASSERT(type == ModelAttribute::POSITIONS || type == ModelAttribute::NORMALS || type == ModelAttribute::TEXCOORDS, "invaltype vertex array type given");
    FEA_ASSERT(mVertexArrays.count(type) == 0, "buffer of that type already added");

    mVertexArrays.emplace(type, vertices);
}

void Model::addBlendArray(ModelAttribute type, const std::vector<uint8_t>& vertices)
{
    FEA_ASSERT(type == ModelAttribute::BLENDWEIGHTS || type == ModelAttribute::BLENDINDICES, "invaltype blend array type given");
    FEA_ASSERT(mBlendArrays.count(type) == 0, "buffer of that type already added");

    mBlendArrays.emplace(type, vertices);
}

const std::vector<float>* Model::findVertexArray(ModelAttribute type) const
{
    FEA_ASSERT(type == ModelAttribute::POSITIONS || type == ModelAttribute::NORMALS || type == ModelAttribute::TEXCOORDS, "invaltype vertex array type given");
    const auto& iterator = mVertexArrays.find(type);

    return iterator == mVertexArrays.end() ? nullptr : &iterator->second;
}

const std::vector<uint8_t>* Model::findBlendArray(ModelAttribute type) const
{
    FEA_ASSERT(type == ModelAttribute::BLENDWEIGHTS || type == ModelAttribute::BLENDINDICES, "invaltype blend array type given");
    const auto& iterator = mBlendArrays.find(type);

    return iterator == mBlendArrays.end() ? nullptr : &iterator->second;
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
        
void Model::addAnimation(const std::string& name, std::unique_ptr<Animation> animation)
{
    mAnimations.emplace(name, std::move(animation));
}

const Animation* Model::findAnimation(const std::string& name) const
{
    const auto& iterator = mAnimations.find(name);

    return iterator == mAnimations.end() ? nullptr : iterator->second.get();
}

void Model::setJointStructure(const std::vector<int32_t>& jointStructure)
{
    mJointStructure = jointStructure;
}

const std::vector<int32_t>& Model::getJointStructure() const
{
    return mJointStructure;
}
