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

//void Camera::moveForward(float speed)
//{
//	glm::vec3 p = mPosition;
//	glm::vec3 dd = mDirection;
//	dd.y = 0.f;
//	if(glm::length2(dd) != 0)
//	dd = glm::normalize(dd);
//	mPosition = p + dd*speed;
// 	update();
//}
//void Camera::strafe(float speed)
//{
//	glm::vec3 v=mDirection;
//	v.y = 0;
//	v.x = -mDirection.z;
//	v.z = mDirection.x;
//	if(glm::length2(v) != 0)
//	v  = glm::normalize(v);
//	v*=speed;
//	mPosition+=v;
// 	update();
//}
//
void Camera::setDirection(const glm::vec3& dir)
{
	mDirection = dir;
	if(glm::length2(mDirection) != 0)
		mDirection = glm::normalize(mDirection);
    update();
}

void Camera::setPosition(const glm::vec3& pos)
{
	mPosition=pos;
    update();
}
//void Camera::addPosition(const glm::vec3& p)
//{
//	mPosition+=p;
//    update();
//}
//
//void Camera::setUpDir(const glm::vec3& upDir)
//{
//	if(glm::length2(upDir) != 0)
//		mUpDirection=glm::normalize(upDir);
//	else 
//		mUpDirection = upDir;
//    update();
//}

void Camera::update()
{
	mMatrix = glm::lookAt(mPosition, mPosition + mDirection * 10.f, mUpDirection);
}

//void Camera::lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
//{
//	setPosition(eye);
//	setDirection(center - eye);
//	setUpDir(up);
//	update();
//}

void Camera::setOrientation(glm::quat orientation)
{
    std::cout << "orienten: " << orientation << "\n";
    glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
	//float pitch = glm::pitch(orientation);
	//float yaw = glm::yaw(orientation);
	//if(pitch >= glm::pi<float>() * 0.5f)
	//	pitch = glm::pi<float>() * 0.5f - 0.001f;
	//if(pitch <= -glm::pi<float>() * 0.5f)
	//	pitch = -glm::pi<float>() * 0.5f + 0.001f;
		
	//setDirection(glm::vec3(glm::cos(pitch)*glm::sin(yaw), glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw)));
	setDirection(forward);
    update();
}

const glm::mat4& Camera::getMatrix() const
{
    return mMatrix;
}
