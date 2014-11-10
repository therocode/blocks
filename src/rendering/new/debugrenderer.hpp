#pragma once
#include "rendermodule.hpp"
#include "debugrenderable.hpp"

class DebugRenderer : public RenderModule
{
        virtual void queue(const Renderable& renderable);
        virtual void render();
        virtual std::type_index getRenderableType() const;
};
