#include "renderer.hpp"
#include "renderable.hpp"
#include "../opengl.hpp"
        
Renderer::Renderer(const GLContext& glContext)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

}

void Renderer::addModule(std::unique_ptr<RenderModule> module)
{
    auto iterator = mModules.emplace(std::move(module)).first;
    mModuleSubscriptions[(*iterator)->getRenderableType()].push_back(&**iterator);
}

void Renderer::queue(const Renderable& renderable)
{
    auto iterator = mModuleSubscriptions.find(renderable.getType());
    if(iterator != mModuleSubscriptions.end())
    {
        for(auto& module : iterator->second)
            module->queue(renderable);
    }
}

void Renderer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(auto& module : mModules)
        module->render();
}
