#include <fea/assert.hpp>
#include "renderingmessages.hpp"
#include "guirenderer.hpp"

GuiRenderer::GuiRenderer(fea::MessageBus& bus):
    mBus(bus)
{
}

void GuiRenderer::queue(const Renderable& renderable)
{
    const GuiRenderable& guiRenderable = (const GuiRenderable&) renderable;
    mGenerator.generate(guiRenderable.element());
}

void GuiRenderer::render(const Camera& camera, const glm::mat4& perspective, const Shader& shader)
{
}

std::type_index GuiRenderer::getRenderableType() const
{
    return typeid(GuiRenderable);
}

void GuiRenderer::textureArrayAdded(uint32_t id, TextureArray& textureArray)
{
    mTextureArrays.emplace(id, textureArray);
}

void GuiRenderer::textureDefinitionAdded(const std::string& name, const TextureDefinition& textureDefinition)
{
    uint32_t id = mGenerator.registerTexture(textureDefinition.size);
    mTextureDefinitions.emplace(id, textureDefinition);

    mBus.send(GuiTextureAddedMessage{id, name, textureDefinition});
}
