#include "debugrenderer.hpp"
#include <iostream>

void DebugRenderer::queue(const Renderable& renderable)
{
    std::cout << "got a renderable! wooh!\n";
}

void DebugRenderer::render()
{
    std::cout << "now rendering\n";
}

std::type_index DebugRenderer::getRenderableType() const
{
    return std::type_index(typeid(DebugRenderable));   
}
