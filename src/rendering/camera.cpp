#include "camera.h"
Camera::Camera()
{
	position[0]=position[1]=position[2]=0;
	direction[0]=direction[1]=0;
	direction[2]=-1.f;
	phi=glm::pi<float>()*0.5f;
	theta=glm::pi<float>()*0.5f;
	upDirection[0]=0;
	upDirection[1]=1.f;
	upDirection[2]=0.0f;
	AddDirection(0.f,0.f);
}
Camera::~Camera()
{

}
glm::vec3 Camera::GetPosition()
{
	return position;
}
glm::vec3 Camera::GetDirection(){
	return direction;
}

void Camera::MoveForward(float speed)
{
	glm::vec3 p = position;
	glm::vec3 dd = direction;
#if 1 
	dd.y = 0.f;
	if(glm::length2(dd) != 0)
	dd = glm::normalize(dd);
#endif
	position = p + dd*speed;
// 	Update();
}
void Camera::Strafe(float speed)
{
	glm::vec3 v=direction;
	v.y = 0;
	v.x = -direction.z;
	v.z = direction.x;
	if(glm::length2(v) != 0)
	v  = glm::normalize(v);
	v*=speed;
	position+=v;
}

void Camera::SetDirection(glm::vec3 dir)
{
	direction = dir;
	if(glm::length2(direction) != 0)
		direction = glm::normalize(direction);
	theta = glm::acos(direction.y);
}
void Camera::SetPosition(glm::vec3 pos)
{
	position=pos;
}
void Camera::AddPosition(glm::vec3 p)
{
	position+=p;
}

void Camera::SetUpDir(glm::vec3 upDir)
{
	if(glm::length2(upDir) != 0)
		upDirection=glm::normalize(upDir);
	else 
		upDirection = upDir;
}

void Camera::Update()
{

	/*float x = position.x, y = position.y, z = position.z;
	matrix = glm::mat4();
	
	float	ax, ay, az;
	float	bx, by, bz;
	float	cx, cy, cz;
	float	r;
	
	cx = -direction[0];
	cy = -direction[1];
	cz = -direction[2];
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
	r =  upDirection[0] * cx + upDirection[1] * cy + upDirection[2] * cz;
	bx = upDirection[0] - r * cx;
	by = upDirection[1] - r * cy;
	bz = upDirection[2] - r * cz;
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
	matrix = glm::lookAt(position, position + direction * 10.f, upDirection);

}

void Camera::LookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
{

	SetPosition(glm::vec3(eyeX,eyeY,eyeZ));
	SetDirection(glm::vec3(centerX-eyeX,centerY-eyeY,centerZ-eyeZ));
	SetUpDir(glm::vec3(upX,upY,upZ));
	Update();
}
void Camera::AddDirection(float x, float y)
{
	phi   +=x;
	theta +=y;
	
	if(theta >= glm::pi<float>() * 0.5f)
		theta = glm::pi<float>() * 0.5f - 0.001f;
	if(theta <= -glm::pi<float>() * 0.5f)
		theta = -glm::pi<float>() * 0.5f + 0.001f;
		
	float fSinTheta = glm::sin(theta);
	float fCosTheta = glm::cos(theta);
	float fCosPhi = glm::cos(-phi);
	float fSinPhi = glm::sin(-phi);
	
	direction.x = (fCosTheta*fSinPhi);
	direction.y = (fSinTheta);
	direction.z = (fCosTheta*fCosPhi);
	if(glm::length2(direction) != 0)
	direction = glm::normalize(direction);
	Update();
}

void Camera::SetPitchYaw(float pitch, float yaw)
{
	//phi	 =0.001;
	//theta=0.001;
	if(pitch >= glm::pi<float>() * 0.5f)
		pitch = glm::pi<float>() * 0.5f - 0.001f;
	if(pitch <= -glm::pi<float>() * 0.5f)
		pitch = -glm::pi<float>() * 0.5f + 0.001f;
		
	SetDirection(glm::vec3(glm::cos(pitch)*glm::sin(yaw), glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw)));
	//AddDirection(pitch, yaw);
	//printf("direction2:%f, %f, %f\n---\n", pitch, yaw,0);//s direction.z);
}
