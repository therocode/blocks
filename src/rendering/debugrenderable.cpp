#include "debugrenderable.hpp"

DebugRenderable::DebugRenderable() : 
    mColor(1.0f, 1.0f, 1.0f)
{
}

std::type_index DebugRenderable::getType() const
{
    return std::type_index(typeid(DebugRenderable));
}

void DebugRenderable::setPosition(const glm::vec3& position)
{
    mPosition = position;
}

const glm::vec3& DebugRenderable::getPosition() const
{
    return mPosition;
}

void DebugRenderable::setColor(const glm::vec3& color)
{
    mColor = color;
}

const glm::vec3& DebugRenderable::getColor() const
{
    return mColor;
}

void DebugRenderable::setPitch(float pitch)
{
	mPitch = pitch;
}

const float DebugRenderable::getPitch() const
{
	return mPitch;
}

void DebugRenderable::setYaw(float yaw)
{
	mYaw = yaw;
}

const float DebugRenderable::getYaw() const
{
	return mYaw;
}