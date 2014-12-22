#pragma once
#include "renderable.hpp"
#include "../utilities/glm.hpp"

class ExtraRenderable : public Renderable
{
    public:
        ExtraRenderable();
        virtual std::type_index getType() const;
        void setPosition(const glm::vec3& position);
        const glm::vec3& getPosition() const;
        void setColor(const glm::vec3& color);
        const glm::vec3& getColor() const;
		void setPitch(float pitch);
    private:
        glm::vec3 mPosition;
        glm::vec3 mColor;
};
