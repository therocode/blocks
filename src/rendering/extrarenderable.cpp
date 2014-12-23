#include "extrarenderable.hpp"

ExtraRenderable::ExtraRenderable() :
    mColor(1.0f, 1.0f, 1.0f)
{
}

std::type_index ExtraRenderable::getType() const
{
    return std::type_index(typeid(ExtraRenderable));
}

void ExtraRenderable::setPosition(const glm::vec3& position)
{
    mPosition = position;
}

const glm::vec3& ExtraRenderable::getPosition() const
{
    return mPosition;
}

void ExtraRenderable::setColor(const glm::vec3& color)
{
    mColor = color;
}

const glm::vec3& ExtraRenderable::getColor() const
{
    return mColor;
}
