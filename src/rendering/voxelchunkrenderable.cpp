#include "voxelchunkrenderable.hpp"

VoxelChunkRenderable::VoxelChunkRenderable() : 
    mModel(nullptr),
    mTexture(nullptr)
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

void VoxelChunkRenderable::setTexture(const Texture& texture)
{
    mTexture = &texture;
}

const Texture* VoxelChunkRenderable::findTexture() const
{
    return mTexture;
}
