#include "camera.hpp"
Camera::Camera()  :
    mDirection(glm::vec3(0.0f, 0.0f, -1.0f))
{
	mUpDirection[0]=0;
	mUpDirection[1]=1.f;
	mUpDirection[2]=0.0f;

    update();
}
Camera::~Camera()
{

}
const glm::vec3& Camera::getPosition() const
{
	return mPosition;
}
const glm::vec3& Camera::getDirection() const
{
	return mDirection;
}

void Camera::setDirection(const glm::vec3& dir, const glm::vec3& up)
{
	mDirection = dir;
	if(glm::length2(mDirection) != 0)
		mDirection = glm::normalize(mDirection);
	mUpDirection = up;
	if(glm::length2(mUpDirection) != 0)
		mUpDirection = glm::normalize(mUpDirection);
    update();
}

void Camera::setPosition(const glm::vec3& pos)
{
	mPosition=pos;
    update();
}

void Camera::update()
{
	mMatrix = glm::lookAt(mPosition, mPosition + mDirection * 10.f, mUpDirection);
}

void Camera::setOrientation(glm::quat orientation)
{
//	if(pitch >= glm::pi<float>() * 0.5f)
//		pitch = glm::pi<float>() * 0.5f - 0.001f;
//	if(pitch <= -glm::pi<float>() * 0.5f)
//		pitch = -glm::pi<float>() * 0.5f + 0.001f;
		
    setDirection(orientation * glm::vec3(0.0f, 0.0f, -1.0f),
                 orientation * glm::vec3(0.0f, 1.0f, 0.0f));
	//setDirection(glm::vec3(glm::cos(pitch)*glm::sin(yaw), glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw)));
    update();
}

const glm::mat4& Camera::getMatrix() const
{
    return mMatrix;
}
