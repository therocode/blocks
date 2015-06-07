#include <fea/assert.hpp>
#include "renderingmessages.hpp"
#include "camera.hpp"
#include "guirenderer.hpp"
#include "shaderattribute.hpp"

GuiRenderer::GuiRenderer(fea::MessageBus& bus):
    mBus(bus),
    mVertexBuffer(Buffer::ARRAY_BUFFER),
    mModelMatrixBuffer1(Buffer::ARRAY_BUFFER),
    mModelMatrixBuffer2(Buffer::ARRAY_BUFFER),
    mModelMatrixBuffer3(Buffer::ARRAY_BUFFER),
    mModelMatrixBuffer4(Buffer::ARRAY_BUFFER),
    mColorBuffer(Buffer::ARRAY_BUFFER),
    mTexCoordBuffer(Buffer::ARRAY_BUFFER),
    mRenderAmount(0)
{
    fea::subscribe(bus, *this);
    mVertexArray.bind();

    mVertexArray.setVertexAttribute(ShaderAttribute::POSITION, 3, mVertexBuffer);
    mVertexArray.setVertexAttribute(ShaderAttribute::NORMAL, 3, mVertexBuffer);
    mVertexArray.setVertexAttribute(ShaderAttribute::TEXCOORD, 2, mTexCoordBuffer);
    mVertexArray.setVertexAttribute(ShaderAttribute::COLOR, 3, mColorBuffer);

    mVertexArray.setInstanceAttribute(MODELMATRIX1, 4, mModelMatrixBuffer1, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX2, 4, mModelMatrixBuffer2, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX3, 4, mModelMatrixBuffer3, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX4, 4, mModelMatrixBuffer4, 1);
	mVertexArray.unbind();

    std::vector<float> data1 = { 1.0f, 0.0f, 0.0f, 0.0f };
    std::vector<float> data2 = { 0.0f, 1.0f, 0.0f, 0.0f };
    std::vector<float> data3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    std::vector<float> data4 = { 0.0f, 0.0f, 0.0f, 1.0f };

    mModelMatrixBuffer1.setData(data1);
    mModelMatrixBuffer2.setData(data2);
    mModelMatrixBuffer3.setData(data3);
    mModelMatrixBuffer4.setData(data4);
}

void GuiRenderer::queue(const Renderable& renderable)
{
    const GuiRenderable& guiRenderable = (const GuiRenderable&) renderable;
    auto renderData = mGenerator.generate(guiRenderable.element());
    mRenderDatas.insert(mRenderDatas.begin(), renderData.begin(), renderData.end());
}

void GuiRenderer::render(const Camera& camera, const glm::mat4& perspective, const Shader& shader)
{
    if(!mRenderDatas.empty())
    {
        mVertexArray.bind();
        shader.setUniform("viewProjectionMatrix", UniformType::MAT4X4, glm::value_ptr(perspective));
        float shadedRatio = 0.0f;
        shader.setUniform("shadedRatio", UniformType::FLOAT, &shadedRatio);

        for(auto& renderData : mRenderDatas)
        {
            if(!renderData.positions.empty())
            {
                FEA_ASSERT(mTextureDefinitions.count(renderData.imageId) > 0, "invalid texture set on gui");

                uint32_t textureId = mTextureArrays.at(mTextureDefinitions.at(renderData.imageId).textureArrayId).getId();
                shader.setUniform("textureArray", UniformType::TEXTURE_ARRAY, &textureId);

                mVertexBuffer.setData(renderData.positions);
                mColorBuffer.setData(renderData.colors);
                mTexCoordBuffer.setData(renderData.texCoords);

                glDrawArrays(GL_TRIANGLES, 0, renderData.positions.size() / 3);
            }

            if(!renderData.textPositions.empty())
            {
                FEA_ASSERT(mFontTextures.count(renderData.textImageId) > 0, "invalid text texture set on gui");

                uint32_t textureId = mFontTextures.at(renderData.textImageId)->getId();
                shader.setUniform("textureArray", UniformType::TEXTURE_ARRAY, &textureId);

                mVertexBuffer.setData(renderData.textPositions);
                mColorBuffer.setData(renderData.textColors);
                mTexCoordBuffer.setData(renderData.textTexCoords);

                glDrawArrays(GL_TRIANGLES, 0, renderData.textPositions.size() / 3);
            }
        }

        mVertexArray.unbind();

        mRenderDatas.clear();
    }
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

PerspectiveMode GuiRenderer::getPerspectiveMode() const
{
    return PerspectiveMode::PERSPECTIVE_2D;
}

void GuiRenderer::handleMessage(const ResourceDeliverMessage<gim::Font>& received)
{
    std::unique_ptr<Texture> texture = std::make_unique<Texture>();
    uint32_t id = mGenerator.registerFontStorage({*received.resource}, TextureAdaptor(*texture));
    mFonts.emplace(id, received.resource);
    mFontTextures.emplace(id, std::move(texture));

    mBus.send(GuiFontAddedMessage{id, received.resourceName, *received.resource});
}
