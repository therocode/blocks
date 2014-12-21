#include "voxelchunkrenderable.hpp"

VoxelChunkRenderable::VoxelChunkRenderable() : 
    mModel(nullptr),
    mTextureArray(nullptr)
{
}

std::type_index VoxelChunkRenderable::getType() const
{
    return std::type_index(typeid(VoxelChunkRenderable));
}

void VoxelChunkRenderable::setModel(const Model& model)
{
    mModel = &model;
}

const Model* VoxelChunkRenderable::findModel() const
{
    return mModel;
}

void VoxelChunkRenderable::setTexture(const TextureArray& textureArray, uint32_t index)
{
    mTextureArray = &textureArray;
    mTextureIndex = index;
}

const TextureArray* VoxelChunkRenderable::findTextureArray() const
{
    return mTextureArray;
}

uint32_t VoxelChunkRenderable::getTextureIndex() const
{
    return mTextureIndex;
}
