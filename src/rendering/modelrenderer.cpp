#include "modelrenderer.hpp"
#include "camera.hpp"
#include <vector>
#include <string>

ModelRenderer::ModelRenderer()
{
}

void ModelRenderer::queue(const Renderable& renderable)
{
}

void ModelRenderer::render(const Camera& camera, const glm::mat4& perspective)
{
}

std::type_index ModelRenderer::getRenderableType() const
{
    return std::type_index(typeid(ModelRenderable));   
}
