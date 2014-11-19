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

void Camera::moveForward(float speed)
{
	glm::vec3 p = mPosition;
	glm::vec3 dd = mDirection;
	dd.y = 0.f;
	if(glm::length2(dd) != 0)
	dd = glm::normalize(dd);
	mPosition = p + dd*speed;
 	update();
}
void Camera::strafe(float speed)
{
	glm::vec3 v=mDirection;
	v.y = 0;
	v.x = -mDirection.z;
	v.z = mDirection.x;
	if(glm::length2(v) != 0)
	v  = glm::normalize(v);
	v*=speed;
	mPosition+=v;
 	update();
}

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
void Camera::addPosition(const glm::vec3& p)
{
	mPosition+=p;
    update();
}

void Camera::setUpDir(const glm::vec3& upDir)
{
	if(glm::length2(upDir) != 0)
		mUpDirection=glm::normalize(upDir);
	else 
		mUpDirection = upDir;
    update();
}

void Camera::update()
{

	/*float x = mPosition.x, y = mPosition.y, z = mPosition.z;
	matrix = glm::mat4();
	
	float	ax, ay, az;
	float	bx, by, bz;
	float	cx, cy, cz;
	float	r;
	
	cx = -mDirection[0];
	cy = -mDirection[1];
	cz = -mDirection[2];
	r = sqrt( cx * cx + cy * cy + cz * cz );
	cx /= r;
	cy /= r;
	cz /= r;
	
	#if 0
	ax = upY * cz - upZ * cy;
	ay = upZ * cx - upX * cz;
	az = upX * cy - upY * cx;
	r = sqrt( ax * ax + ay * ay + az * az );
	ax /= r;
	ay /= r;
	az /= r;
	
	bx = cy * az - cz * ay;
	by = cz * ax - cx * az;
	bz = cx * ay - cy * ax;
	r = sqrt( bx * bx + by * by + bz * bz );
	bx /= r;
	by /= r;
	bz /= r;
	#else
	r =  mUpDirection[0] * cx + mUpDirection[1] * cy + mUpDirection[2] * cz;
	bx = mUpDirection[0] - r * cx;
	by = mUpDirection[1] - r * cy;
	bz = mUpDirection[2] - r * cz;
	r = sqrt( bx * bx + by * by + bz * bz );
	bx /= r;
	by /= r;
	bz /= r;
	
	ax = by * cz - bz * cy;
	ay = bz * cx - bx * cz;
	az = bx * cy - by * cx;
	r = sqrt( ax * ax + ay * ay + az * az );
	ax /= r;
	ay /= r;
	az /= r;
	#endif	
	
	matrix[0][ 0 ] = ax;
	matrix[0][ 1 ] = bx;
	matrix[0][ 2 ] = cx;
	matrix[0][ 3 ] = 0.0f;
	
	matrix[0][ 0 ] = ay;
	matrix[0][ 1 ] = by;
	matrix[0][ 2 ] = cy;
	matrix[0][ 3 ] = 0.0f;
	
	matrix[0][ 0 ] = az;
	matrix[0][ 1 ] = bz;
	matrix[0][ 2 ] = cz;
	matrix[0][ 3 ] = 0.0f;
	
	matrix[0][ 0 ] = -( ax * x + ay * y + az * z );
	matrix[0][ 1 ] = -( bx * x + by * y + bz * z );
	matrix[0][ 2 ] = -( cx * x + cy * y + cz * z );
	matrix[0][ 3 ] = 1.0f;*/
	mMatrix = glm::lookAt(mPosition, mPosition + mDirection * 10.f, mUpDirection);

}

void Camera::lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
	setPosition(eye);
	setDirection(center - eye);
	setUpDir(up);
	update();
}

void Camera::setPitchYaw(float pitch, float yaw)
{
	if(pitch >= glm::pi<float>() * 0.5f)
		pitch = glm::pi<float>() * 0.5f - 0.001f;
	if(pitch <= -glm::pi<float>() * 0.5f)
		pitch = -glm::pi<float>() * 0.5f + 0.001f;
		
	setDirection(glm::vec3(glm::cos(pitch)*glm::sin(yaw), glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw)));
    update();
}

const glm::mat4& Camera::getMatrix() const
{
    return mMatrix;
}
