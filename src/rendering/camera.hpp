#pragma once
#include "../utilities/glm.hpp"

class Camera 
{

	public:
		Camera();
		~Camera();
		const glm::vec3& getPosition();
		const glm::vec3& getDirection();

		void addDirection(float x, float y);
		void setPosition(const glm::vec3& pos);
		void addPosition(const glm::vec3& p);
		void setDirection(const glm::vec3& dir);
		void moveForward(float speed);
		void strafe(float speed);
		void setUpDir(const glm::vec3& upDir);
		void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
		void update();
		void setPitchYaw(float pitch, float yaw);
		
		const glm::mat4& getMatrix();
	private:
		glm::mat4 mMatrix;
		glm::vec3 mPosition;
		glm::vec3 mDirection;
		glm::vec3 mUpDirection;
		float mPhi;
        float mTheta;
};
