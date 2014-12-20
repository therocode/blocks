#pragma once
#include "renderable.hpp"
#include "../utilities/glm.hpp"

class DebugRenderable : public Renderable
{
    public:
		enum DebugShape {CUBE, LINE};

        DebugRenderable(DebugShape shape);
        virtual std::type_index getType() const;
        void setPosition(const glm::vec3& position);
        const glm::vec3& getPosition() const;
        void setColor(const glm::vec3& color);
        const glm::vec3& getColor() const;
		void setPitch(float pitch);
		const float getPitch() const;
		void setYaw(float yaw);
		const float getYaw() const;

		void setLineColors(const glm::vec3& startColor, const glm::vec3& endColor);
		const glm::vec3& getLineStartColor() const;
		const glm::vec3& getLineEndColor() const;
		void setLinePoints(const glm::vec3& startPoint, const glm::vec3& endPoint);
		const glm::vec3& getLineStartPoint() const;
		const glm::vec3& getLineEndPoint() const;
		const DebugShape getShape() const;
    private:
        glm::vec3 mPosition;
        glm::vec3 mColor;
		float mPitch;
		float mYaw;
		glm::vec3 mLineStartPoint;
		glm::vec3 mLineEndPoint;
		glm::vec3 mLineStartColor;
		glm::vec3 mLineEndColor;
		DebugShape mDebugShape;
};
