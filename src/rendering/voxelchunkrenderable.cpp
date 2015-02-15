#include "voxelchunkrenderable.hpp"

VoxelChunkRenderable::VoxelChunkRenderable() : 
    mModel(nullptr),
    mTextureArray(nullptr),
    mUpdated(false)
{
}

std::type_index VoxelChunkRenderable::getType() const
{
    return std::type_index(typeid(VoxelChunkRenderable));
}

void VoxelChunkRenderable::setModel(const ChunkModel& model)
{
    mModel = &model;
}

const ChunkModel* VoxelChunkRenderable::findModel() const
{
    return mModel;
}

void VoxelChunkRenderable::setTexture(const TextureArray& textureArray)
{
    mTextureArray = &textureArray;
}

const TextureArray* VoxelChunkRenderable::findTextureArray() const
{
    return mTextureArray;
}

void VoxelChunkRenderable::setCoordinate(const ChunkCoord& coordinate)
{
    mCoordinate = coordinate;
}

const ChunkCoord& VoxelChunkRenderable::getCoordinate() const
{
    return mCoordinate;
}

void VoxelChunkRenderable::updated()
{
    mUpdated = true;
}

bool VoxelChunkRenderable::isUpdated() const
{
    return mUpdated;
}
