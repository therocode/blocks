#pragma once
#include <fea/util.hpp>
#include "renderer.hpp"
#include "glcontext.hpp"

class RenderingSystem
{
    public:
        RenderingSystem(fea::MessageBus& bus);
        void render();
    private:
        fea::MessageBus& mBus;
        GLContext mGLContext;
        Renderer mRenderer;
};
