#include "rendermodule.hpp"
#include <iostream>

RenderModule::RenderModule() :
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
        if(mRenderMode)
            mRenderMode->activate();

        render(camera, perspective);

        if(mRenderMode)
            mRenderMode->deactivate();
    }
}

const RenderMode* RenderModule::findRenderMode() const
{
    return mRenderMode.get();
}

RenderMode* RenderModule::findRenderMode()
{
    return mRenderMode.get();
}

void RenderModule::setRenderMode(const RenderMode& renderMode)
{
    mRenderMode = std::unique_ptr<RenderMode>(new RenderMode(renderMode));
}

bool RenderModule::isEnabled() const
{
    return mEnabled;
}

void RenderModule::setEnabled(bool enabled)
{
    mEnabled = enabled;
}
