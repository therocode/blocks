#ifndef CAMERA_H
#define CAMERA_H
#include "../blockstd.hpp"

//!A camera that converts stuff from world space to camera space. or something like that.
class Camera 
{

	public:
		Camera();
		~Camera();
		glm::vec3 GetPosition();
		glm::vec3 GetDirection();

		//!Adds to the phi and theta, which are used setting orientation
		void AddDirection(float x, float y);
		//!Sets the position of the camera.
		void SetPosition(glm::vec3 pos);
		//!Adds to the position.
		void AddPosition(glm::vec3 p);
		//!sets the looking direction of the camera.
		void SetDirection(glm::vec3 dir);
		//!Moves in the camera direction.
		void MoveForward(float speed);
		//!Strafes the camera horizontally, positive value goes right, negative left.
		void Strafe(float speed);
		//!Sets up direction
		void SetUpDir(glm::vec3 upDir);
		//!Works like gluLookAt, kind of.
		void LookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY,float centerZ, float upX, float upY, float upZ);
		//!Updates the camera matrix.
		void Update();
		//!Sets pitch and yaw for the camera.
		void SetPitchYaw(float pitch, float yaw);
		
		glm::mat4 GetMatrix(){
			return matrix;
		};
	private:
		glm::mat4 matrix;
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 upDirection;
		float phi, theta;
};

#endif // CAMERA_H
