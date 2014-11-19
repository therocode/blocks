#include "rendermode.hpp"
#include <fea/assert.hpp>

const int32_t defaultPolygonMode = GL_FILL;

RenderMode::RenderMode() :
    mPolygonMode(FILL)
{
}

PolygonMode RenderMode::getPolygonMode() const
{
    return mPolygonMode;
}

void RenderMode::setPolygonMode(PolygonMode polygonMode)
{
    FEA_ASSERT(polygonMode == FILL || polygonMode == LINE || polygonMode == POINT, "Invalid polygon mode given");

    mPolygonMode = polygonMode;
}

void RenderMode::activate() const
{
    GLint mode;

    if(mPolygonMode == FILL)
        mode = GL_FILL;
    else if(mPolygonMode == LINE)
        mode = GL_LINE;
    else
        mode = GL_POINT;

    glPolygonMode(GL_FRONT_AND_BACK, mode);
}

void RenderMode::deactivate() const
{
    glPolygonMode(GL_FRONT_AND_BACK, defaultPolygonMode);
}
