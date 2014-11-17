#pragma once
#include <typeindex>
#include "../../utilities/glm.hpp"

class Renderable;
class Camera;

class RenderModule
{
    public:
        virtual ~RenderModule();
        virtual void queue(const Renderable& renderable) = 0;
        virtual void render(const Camera& camera, const glm::mat4& perspective) = 0;
        virtual std::type_index getRenderableType() const = 0;
};
