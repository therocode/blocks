#pragma once
#include "renderable.hpp"
#include "../utilities/glm.hpp"

class ModelRenderable : public Renderable
{
    public:
        ModelRenderable();
        virtual std::type_index getType() const;
        void setPosition(const glm::vec3& position);
        const glm::vec3& getPosition() const;
        void setColor(const glm::vec3& color);
        const glm::vec3& getColor() const;
    private:
        glm::vec3 mPosition;
        glm::vec3 mColor;
};
