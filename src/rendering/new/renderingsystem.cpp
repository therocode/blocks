#include "renderingsystem.hpp"
#include "debugrenderer.hpp"

RenderingSystem::RenderingSystem(fea::MessageBus& bus) :
    mBus(bus),
    mRenderer(mGLContext)

{
    mRenderer.addModule(std::unique_ptr<DebugRenderer>(new DebugRenderer()));
}

void RenderingSystem::render()
{
    DebugRenderable debug;
    DebugRenderable debug2;
    DebugRenderable debug3;

    debug.setPosition({0.0f, 0.0f, 0.0f});
    debug.setColor({1.0f, 0.0f, 0.4f});

    debug2.setPosition({0.3f, 0.0f, 0.0f});
    debug2.setColor({0.0f, 1.0f, 0.4f});

    debug3.setPosition({0.6f, 0.0f, 0.0f});
    debug3.setColor({0.4f, 0.0f, 1.0f});

    mRenderer.queue(debug);
    mRenderer.queue(debug2);
    mRenderer.queue(debug3);
    mRenderer.render();
}
