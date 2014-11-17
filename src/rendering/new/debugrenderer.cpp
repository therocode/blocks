#include "debugrenderer.hpp"
#include "camera.hpp"
#include <vector>
#include <string>
#include <iostream>

DebugRenderer::DebugRenderer()
{
    std::string vertexSource = R"(
#version 150

uniform mat4 viewProjectionMatrix;

attribute vec3 in_position;
attribute vec4 modelMatrix1;
attribute vec4 modelMatrix2;
attribute vec4 modelMatrix3;
attribute vec4 modelMatrix4;
attribute vec3 color;

out vec3 objectColor;

void main()
{
    mat4 modelMatrix = mat4( modelMatrix1, modelMatrix2, modelMatrix3, modelMatrix4 );
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(vec3(in_position.x, in_position.y, in_position.z), 1.0);
    objectColor = color;
})";

    std::string fragmentSource = R"(
#version 150
precision highp float;

in vec3 objectColor;
out vec4 fragColor;

void main()
{
    fragColor = vec4(objectColor, 1.0f);
})";

    mShader.setSource(vertexSource, fragmentSource);
    mShader.compile();

    std::vector<float> vertices = {
             -1.0f, -1.0f, -2.0f,
            1.0f, -1.0f, -2.0f,
               0.0f,  1.0f, -2.0f,
    };

    mVertexArray.bind();
    mVertexBuffer.setData(vertices);
    mShader.setVertexAttribute("in_position", 3, mVertexBuffer);
}

void DebugRenderer::queue(const Renderable& renderable)
{
    const DebugRenderable& debugRenderable = (const DebugRenderable&) renderable;
    
    const glm::vec3& position = debugRenderable.getPosition();
    const glm::vec3& color = debugRenderable.getColor();

    std::vector<float> data1 = { 1.0f, 0.0f, 0.0f, 0.0f };
    std::vector<float> data2 = { 0.0f, 1.0f, 0.0f, 0.0f };
    std::vector<float> data3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    std::vector<float> data4 = { position.x, position.y, position.z , 1.0f };

    mModelMatrixData1.insert(mModelMatrixData1.end(), data1.begin(), data1.end());
    mModelMatrixData2.insert(mModelMatrixData2.end(), data2.begin(), data2.end());
    mModelMatrixData3.insert(mModelMatrixData3.end(), data3.begin(), data3.end());
    mModelMatrixData4.insert(mModelMatrixData4.end(), data4.begin(), data4.end());
    mColorData.push_back(color.x);
    mColorData.push_back(color.y);
    mColorData.push_back(color.z);
}

void DebugRenderer::render(const Camera& camera, const glm::mat4& perspective)
{
    uint32_t renderAmount = mModelMatrixData1.size() / 4;
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

    mShader.setInstanceAttribute("modelMatrix1", 4, mModelMatrixBuffer1, 1);
    mShader.setInstanceAttribute("modelMatrix2", 4, mModelMatrixBuffer2, 1);
    mShader.setInstanceAttribute("modelMatrix3", 4, mModelMatrixBuffer3, 1);
    mShader.setInstanceAttribute("modelMatrix4", 4, mModelMatrixBuffer4, 1);
    mShader.setInstanceAttribute("color", 3, mColorBuffer, 1);

    mShader.setUniform("viewProjectionMatrix", MAT4X4, glm::value_ptr(perspective * camera.getMatrix()));

    mShader.activate();

    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, renderAmount);
}

std::type_index DebugRenderer::getRenderableType() const
{
    return std::type_index(typeid(DebugRenderable));   
}
