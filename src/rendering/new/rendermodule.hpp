#pragma once
#include <typeindex>

class RenderModule
{
    public:
        virtual void queue(const Renderable& renderable) = 0;
        virtual void render() = 0;
        virtual std::type_index getRenderableType() const = 0;
};
