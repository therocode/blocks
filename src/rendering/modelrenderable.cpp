#include "modelrenderable.hpp"

ModelRenderable::ModelRenderable() : 
    mColor(1.0f, 1.0f, 1.0f),
    mModel(nullptr)
{
}

std::type_index ModelRenderable::getType() const
{
    return std::type_index(typeid(ModelRenderable));
}

void ModelRenderable::setPosition(const glm::vec3& position)
{
    mPosition = position;
}

const glm::vec3& ModelRenderable::getPosition() const
{
    return mPosition;
}

void ModelRenderable::setColor(const glm::vec3& color)
{
    mColor = color;
}

const glm::vec3& ModelRenderable::getColor() const
{
    return mColor;
}

void ModelRenderable::setModel(const Model& model)
{
    mModel = &model;
}

const Model* ModelRenderable::findModel() const
{
    return mModel;
}

void ModelRenderable::setTexture(const TextureArray& textureArray, uint32_t index)
{
    mTextureArray = &textureArray;
    mTextureIndex = index;
}

const TextureArray* ModelRenderable::findTextureArray() const
{
    return mTextureArray;
}

uint32_t ModelRenderable::getTextureIndex() const
{
    return mTextureIndex;
}
