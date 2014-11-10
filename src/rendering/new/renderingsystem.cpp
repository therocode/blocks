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

    mRenderer.queue(debug);
    mRenderer.render();
}
