#pragma once
#include "../../utilities/glm.hpp"

class Camera 
{

	public:
		Camera();
		~Camera();
		const glm::vec3& getPosition() const;
		const glm::vec3& getDirection() const;

		void setPosition(const glm::vec3& pos);
		void addPosition(const glm::vec3& p);
		void setDirection(const glm::vec3& dir);
		void moveForward(float speed);
		void strafe(float speed);
		void setUpDir(const glm::vec3& upDir);
		void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
		void setPitchYaw(float pitch, float yaw);
		
		const glm::mat4& getMatrix() const;
	private:
		void update();
		glm::mat4 mMatrix;
		glm::vec3 mPosition;
		glm::vec3 mDirection;
		glm::vec3 mUpDirection;
};
