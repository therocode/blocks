#include "rendermodule.hpp"
#include <iostream>

RenderModule::RenderModule() :
    mModeEnabled(false),
    mEnabled(true)
{
}

RenderModule::~RenderModule()
{
}

void RenderModule::metaQueue(const Renderable& renderable)
{
    if(mEnabled)
        queue(renderable);
}

void RenderModule::metaRender(const Camera& camera, const glm::mat4& perspective, const Shader& shader)
{
    if(mEnabled)
    {
        if(mModeEnabled)
            mRenderMode.activate();

        render(camera, perspective, shader);

        if(mModeEnabled)
            mRenderMode.deactivate();
    }
}

void RenderModule::enableRenderMode(bool enabled)
{
    mModeEnabled = enabled;
}

const RenderMode& RenderModule::getRenderMode() const
{
    return mRenderMode;
}

RenderMode& RenderModule::getRenderMode()
{
    return mRenderMode;
}

void RenderModule::setRenderMode(const RenderMode& renderMode)
{
    mRenderMode = renderMode;
}

bool RenderModule::isEnabled() const
{
    return mEnabled;
}

void RenderModule::setEnabled(bool enabled)
{
    mEnabled = enabled;
}

void RenderModule::textureArrayAdded(uint32_t id, TextureArray& textureArray)
{

}

void RenderModule::textureDefinitionAdded(const std::string& name, const TextureDefinition& textureDefinition)
{
}

PerspectiveMode RenderModule::getPerspectiveMode() const
{
    return PerspectiveMode::PERSPECTIVE_3D;
}
