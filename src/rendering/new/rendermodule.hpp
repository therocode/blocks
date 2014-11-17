#pragma once
#include <typeindex>

class Renderable;
class Camera;

class RenderModule
{
    public:
        virtual ~RenderModule();
        virtual void queue(const Renderable& renderable) = 0;
        virtual void render(const Camera& camera) = 0;
        virtual std::type_index getRenderableType() const = 0;
};
