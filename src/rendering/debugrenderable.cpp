#include "debugrenderable.hpp"

DebugRenderable::DebugRenderable(DebugShape shape) :
    mColor(1.0f, 1.0f, 1.0f),
    mPitch(0.0f),
    mYaw(0.0f)
{
	mDebugShape = shape;
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

void DebugRenderable::setLineColors(const glm::vec3& startColor, const glm::vec3& endColor)
{
	mLineStartColor = startColor;
	mLineEndColor = endColor;
}

const glm::vec3& DebugRenderable::getLineStartColor() const
{
	return mLineStartColor;
}

const glm::vec3& DebugRenderable::getLineEndColor() const
{
	return mLineEndColor;
}

void DebugRenderable::setLinePoints(const glm::vec3& startPoint, const glm::vec3& endPoint)
{
	mLineStartPoint = startPoint;
	mLineEndPoint = endPoint;
}

const glm::vec3& DebugRenderable::getLineStartPoint() const
{
	return mLineStartPoint;
}

const glm::vec3& DebugRenderable::getLineEndPoint() const
{
	return mLineEndPoint;
}

const DebugRenderable::DebugShape DebugRenderable::getShape() const
{
	return mDebugShape;
}
