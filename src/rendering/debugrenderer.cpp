#include "debugrenderer.hpp"
#include "camera.hpp"
#include <vector>
#include <string>
#include <iostream>

DebugRenderer::DebugRenderer() :
    mRenderAmount(0)
{

    std::string vertexSource = R"(
#version 330

uniform mat4 viewProjectionMatrix;

layout(location = ~POSITION~) in vec3 in_position;
layout(location = ~COLOR~) in vec3 color;
layout(location = ~MODELMATRIX1~) in vec4 modelMatrix1;
layout(location = ~MODELMATRIX2~) in vec4 modelMatrix2;
layout(location = ~MODELMATRIX3~) in vec4 modelMatrix3;
layout(location = ~MODELMATRIX4~) in vec4 modelMatrix4;

out vec3 objectColor;

void main()
{
    mat4 modelMatrix = mat4( modelMatrix1, modelMatrix2, modelMatrix3, modelMatrix4 );
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(vec3(in_position.x, in_position.y, in_position.z), 1.0);
    objectColor = color;
})";

    std::string fragmentSource = R"(
#version 330
precision highp float;

in vec3 objectColor;
out vec4 fragColor;

void main()
{
    fragColor = vec4(objectColor, 1.0f);
})";

    mShader.setSource(vertexSource, fragmentSource);
    mShader.compile({
            {"POSITION", DebugAttribute::POSITION},
            {"COLOR", DebugAttribute::COLOR},
            {"MODELMATRIX1", DebugAttribute::MODELMATRIX1},
            {"MODELMATRIX2", DebugAttribute::MODELMATRIX2},
            {"MODELMATRIX3", DebugAttribute::MODELMATRIX3},
            {"MODELMATRIX4", DebugAttribute::MODELMATRIX4}
            });

    std::vector<float> vertices = {
             -0.5f, -0.5f,  0.5f,
              0.5f, -0.5f,  0.5f,
             -0.5f,  0.5f,  0.5f,

             -0.5f,  0.5f,  0.5f,
              0.5f, -0.5f,  0.5f,
              0.5f,  0.5f,  0.5f,


             -0.5f, -0.5f, -0.5f,
             -0.5f,  0.5f, -0.5f,
              0.5f, -0.5f, -0.5f,

             -0.5f,  0.5f, -0.5f,
              0.5f,  0.5f, -0.5f,
              0.5f, -0.5f, -0.5f,



             -0.5f, -0.5f,  0.5f,
             -0.5f,  0.5f,  0.5f,
             -0.5f, -0.5f, -0.5f,

             -0.5f, -0.5f, -0.5f,
             -0.5f,  0.5f,  0.5f,
             -0.5f,  0.5f, -0.5f,


              0.5f, -0.5f,  0.5f,
              0.5f, -0.5f, -0.5f,
              0.5f,  0.5f,  0.5f,

              0.5f, -0.5f, -0.5f,
              0.5f,  0.5f, -0.5f,
              0.5f,  0.5f,  0.5f,



             -0.5f,  0.5f,  0.5f,
              0.5f,  0.5f,  0.5f,
             -0.5f,  0.5f, -0.5f,

              0.5f,  0.5f,  0.5f,
              0.5f,  0.5f, -0.5f,
             -0.5f,  0.5f, -0.5f,


             -0.5f, -0.5f,  0.5f,
             -0.5f, -0.5f, -0.5f,
              0.5f, -0.5f,  0.5f,

              0.5f, -0.5f,  0.5f,
             -0.5f, -0.5f, -0.5f,
              0.5f, -0.5f, -0.5f,
    };

    mVertexArray.bind();
    mVertexBuffer.setData(vertices);

    mVertexArray.setVertexAttribute(DebugAttribute::POSITION, 3, mVertexBuffer);

    mVertexArray.setInstanceAttribute(MODELMATRIX1, 4, mModelMatrixBuffer1, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX2, 4, mModelMatrixBuffer2, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX3, 4, mModelMatrixBuffer3, 1);
    mVertexArray.setInstanceAttribute(MODELMATRIX4, 4, mModelMatrixBuffer4, 1);
    mVertexArray.setInstanceAttribute(COLOR, 3, mColorBuffer, 1);

    data1 = { 1.0f, 0.0f, 0.0f, 0.0f };
    data2 = { 0.0f, 1.0f, 0.0f, 0.0f };
    data3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    data4 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

void DebugRenderer::queue(const Renderable& renderable)
{
    const DebugRenderable& debugRenderable = (const DebugRenderable&) renderable;
    
    const glm::vec3& position = debugRenderable.getPosition();
    const glm::vec3& color = debugRenderable.getColor();

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

void DebugRenderer::render(const Camera& camera, const glm::mat4& perspective)
{
    mVertexArray.bind();
    mShader.activate();

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

    mShader.setUniform("viewProjectionMatrix", UniformType::MAT4X4, glm::value_ptr(perspective * camera.getMatrix()));

    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mRenderAmount);
    mRenderAmount = 0;

    mShader.deactivate();
    mVertexArray.unbind();
}

std::type_index DebugRenderer::getRenderableType() const
{
    return std::type_index(typeid(DebugRenderable));   
}
