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

void RenderModule::metaRender(const Camera& camera, const glm::mat4& perspective)
{
    if(mEnabled)
    {
        if(mModeEnabled)
            mRenderMode.activate();

        render(camera, perspective);

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