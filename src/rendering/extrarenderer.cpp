#include "extrarenderer.hpp"
#include "shapes.hpp"
#include "shaderattribute.hpp"
#include "camera.hpp"
#include <vector>
#include <string>
#include <iostream>
#include "../resources/image.hpp"

ExtraRenderer::ExtraRenderer() :
    mRenderAmount(0)
{
    mVertexArray.bind();
    mVertexBuffer.setData(lineCageVertices);

    mVertexArray.setVertexAttribute(ShaderAttribute::POSITION, 3, mVertexBuffer);
    mVertexArray.setVertexAttribute(ShaderAttribute::NORMAL, 3, mVertexBuffer);
    //mVertexArray.setVertexAttribute(ShaderAttribute::TEXCOORD, 2, mVertexBuffer);

    mVertexArray.setInstanceAttribute(MODELMATRIX1, 4, mModelMatrixBuffer1, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX2, 4, mModelMatrixBuffer2, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX3, 4, mModelMatrixBuffer3, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX4, 4, mModelMatrixBuffer4, 1);

    mVertexArray.setInstanceAttribute(COLOR, 3, mColorBuffer, 1);
	mVertexArray.unbind();

    data1 = { 1.0f, 0.0f, 0.0f, 0.0f };
    data2 = { 0.0f, 1.0f, 0.0f, 0.0f };
    data3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    data4 = { 0.0f, 0.0f, 0.0f, 1.0f };

    Image image({16, 16}, fea::Color::White);
    mWhiteTextureArray.create({16, 16}, 1, image.getPixelsPointer());
}

void ExtraRenderer::queue(const Renderable& renderable)
{
    const ExtraRenderable& extraRenderable = (const ExtraRenderable&) renderable;

    const glm::vec3& position = extraRenderable.getPosition();
    const glm::vec3& color = extraRenderable.getColor();

    data4[0] = position.x;
    data4[1] = position.y;
    data4[2] = position.z;

    mModelMatrixData1.insert(mModelMatrixData1.end(), data1.begin(), data1.end());
    mModelMatrixData2.insert(mModelMatrixData2.end(), data2.begin(), data2.end());
    mModelMatrixData3.insert(mModelMatrixData3.end(), data3.begin(), data3.end());
    mModelMatrixData4.insert(mModelMatrixData4.end(), data4.begin(), data4.end());

    mColorData.push_back(color.x);
    mColorData.push_back(color.y);
    mColorData.push_back(color.z);

    mRenderAmount++;
}

void ExtraRenderer::render(const Camera& camera, const glm::mat4& perspective, const Shader& shader)
{
    GLuint id = mWhiteTextureArray.getId();
    
    shader.setUniform("textureArray", UniformType::TEXTURE_ARRAY, &id);

    if(!mModelMatrixData1.empty())
    {
        mVertexArray.bind();

        mModelMatrixBuffer1.setData(mModelMatrixData1);
        mModelMatrixBuffer2.setData(mModelMatrixData2);
        mModelMatrixBuffer3.setData(mModelMatrixData3);
        mModelMatrixBuffer4.setData(mModelMatrixData4);

        mColorBuffer.setData(mColorData);

        mModelMatrixData1.clear();
        mModelMatrixData2.clear();
        mModelMatrixData3.clear();
        mModelMatrixData4.clear();

        mColorData.clear();

        shader.setUniform("viewProjectionMatrix", UniformType::MAT4X4, glm::value_ptr(perspective * camera.getMatrix()));
        float shadedRatio = 0.0f;
        shader.setUniform("shadedRatio", UniformType::FLOAT, &shadedRatio);

        glDrawArraysInstanced(GL_LINES, 0, 24, mRenderAmount);
        mRenderAmount = 0;

        mVertexArray.unbind();
    }
}

std::type_index ExtraRenderer::getRenderableType() const
{
    return std::type_index(typeid(ExtraRenderable));   
}
