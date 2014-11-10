#include "renderer.hpp"
#include "renderable.hpp"
        
Renderer::Renderer(const GLContext& glContext)
{
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
    for(auto& module : mModules)
        module->render();
}
