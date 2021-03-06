#include "renderer.hpp"
#include "renderable.hpp"
#include "opengl.hpp"
#include <fea/assert.hpp>
        
Renderer::Renderer(const GLContext& glContext, const glm::uvec2& viewSize) :
    mViewSize(viewSize),
    mFov(90.0f),
    mNear(0.1f),
    mFar(10000.0f),
    mEnabled(true)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glClearColor(mClearColor.r, mClearColor.g, mClearColor.b, 0.0f);

    setViewSize(mViewSize);
}

void Renderer::addModule(int32_t id, std::unique_ptr<RenderModule> module)
{
    auto iterator = mModules.emplace(id, std::move(module)).first;
    mModuleSubscriptions[(*iterator).second->getRenderableType()].push_back(&*iterator->second);
}

void Renderer::queue(const Renderable& renderable)
{
    if(mEnabled)
    {
        auto iterator = mModuleSubscriptions.find(renderable.getType());
        if(iterator != mModuleSubscriptions.end())
        {
            for(auto& module : iterator->second)
                module->metaQueue(renderable);
        }
    }
}

void Renderer::clear()
{
    glClearColor(mClearColor.r, mClearColor.g, mClearColor.b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(const Shader& shader)
{
    if(mEnabled)
    {
        shader.activate();
        for(auto& module : mModules)
        {
            mRenderMode.activate();

            glm::mat4* perspective = nullptr;
            if(module.second->getPerspectiveMode() == PerspectiveMode::PERSPECTIVE_3D)
                perspective = &mPerspective3D;
            else if(module.second->getPerspectiveMode() == PerspectiveMode::PERSPECTIVE_2D)
                perspective = &mPerspective2D;

            module.second->metaRender(mCamera, *perspective, shader);
        }

        mRenderMode.deactivate();
        shader.deactivate();
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
    mViewSize = size;
	glViewport(0, 0, size.x, size.y);

    setPerspective2D(size);
    setPerspective3D(mFov, mNear, mFar);
}

const RenderMode& Renderer::getRenderMode() const
{
    return mRenderMode;
}

RenderMode& Renderer::getRenderMode()
{
    return mRenderMode;
}

void Renderer::enableRenderMode(int32_t moduleId, bool enabled)
{
    FEA_ASSERT(mModules.count(moduleId) != 0, "render module id " + std::to_string(moduleId) + " does not exist");

    mModules.at(moduleId)->enableRenderMode(enabled);
}

void Renderer::setRenderMode(const RenderMode& renderMode)
{
    mRenderMode = renderMode;
}

const RenderMode& Renderer::getRenderMode(int32_t moduleId) const
{
    FEA_ASSERT(mModules.count(moduleId) != 0, "render module id " + std::to_string(moduleId) + " does not exist");

    return mModules.at(moduleId)->getRenderMode();
}

RenderMode& Renderer::getRenderMode(int32_t moduleId)
{
    FEA_ASSERT(mModules.count(moduleId) != 0, "render module id " + std::to_string(moduleId) + " does not exist");

    return mModules.at(moduleId)->getRenderMode();
}

void Renderer::setRenderMode(int32_t moduleId, const RenderMode& renderMode)
{
    FEA_ASSERT(mModules.count(moduleId) != 0, "render module id " + std::to_string(moduleId) + " does not exist");

    mModules.at(moduleId)->setRenderMode(renderMode);
}

bool Renderer::isEnabled() const
{
    return mEnabled;
}

void Renderer::setEnabled(bool enabled)
{
    mEnabled = enabled;
}

bool Renderer::isEnabled(int32_t moduleId) const
{
    FEA_ASSERT(mModules.count(moduleId) != 0, "render module id " + std::to_string(moduleId) + " does not exist");

    return mModules.at(moduleId)->isEnabled();
}

void Renderer::setEnabled(int32_t moduleId, bool enabled)
{
    FEA_ASSERT(mModules.count(moduleId) != 0, "render module id " + std::to_string(moduleId) + " does not exist");

    mModules.at(moduleId)->setEnabled(enabled);
}

void Renderer::setPerspective3D(float fov, float rnear, float rfar)
{
    mFov = fov;
    mNear = rnear;
    mFar = rfar;

    mPerspective3D = glm::perspective(glm::radians(mFov), (float)mViewSize.x / (float)mViewSize.y, mNear, mFar);
}

void Renderer::setPerspective2D(const glm::uvec2& size)
{
    mPerspective2D = glm::ortho(0.0f, (float)size.x, (float)size.y, 0.0f, mNear, mFar);
}

void Renderer::setClearColor(const glm::vec3& color)
{
    mClearColor = color;
}

const glm::vec3& Renderer::getClearColor() const
{
    return mClearColor;
}

void Renderer::addTextureArray(uint32_t id, TextureArray& textureArray)
{
    for(auto& module : mModules)
        module.second->textureArrayAdded(id, textureArray);
}

void Renderer::addTextureDefinition(const std::string& name, const TextureDefinition& textureDefinition)
{
    for(auto& module : mModules)
        module.second->textureDefinitionAdded(name, textureDefinition);
}
