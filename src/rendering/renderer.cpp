#include "renderer.hpp"
#include "renderable.hpp"
#include "opengl.hpp"
        
Renderer::Renderer(const GLContext& glContext)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
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
    {
        mRenderMode.activate();
        module->render(mCamera, mPerspective);
    }

    mRenderMode.deactivate();
}

const Camera& Renderer::getCamera() const
{
    return mCamera;
}

Camera& Renderer::getCamera()
{
    return mCamera;
}

void Renderer::setCamera(const Camera& camera)
{
    mCamera = camera;
}

void Renderer::setViewSize(const glm::uvec2& size)
{
	glViewport(0, 0, size.x, size.y);

    mPerspective = glm::perspective(glm::radians(90.0f), (float)size.x / (float)size.y, 0.1f, 100.0f);
}

const RenderMode& Renderer::getRenderMode() const
{
    return mRenderMode;
}

RenderMode& Renderer::getRenderMode()
{
    return mRenderMode;
}

void Renderer::setRenderMode(const RenderMode& renderMode)
{
    mRenderMode = renderMode;
}
