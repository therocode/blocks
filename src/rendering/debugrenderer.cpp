#include "debugrenderer.hpp"
#include "shaderattribute.hpp"
#include "camera.hpp"
#include <vector>
#include <string>
#include <iostream>

DebugRenderer::DebugRenderer() :
    mRenderAmount(0)
{
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

    mVertexArray.setVertexAttribute(ShaderAttribute::POSITION, 3, mVertexBuffer);
    mVertexArray.setVertexAttribute(ShaderAttribute::NORMAL, 3, mVertexBuffer);

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
	const float pitch = debugRenderable.getPitch();
	const float yaw = debugRenderable.getYaw();

    data4[0] = position.x;
    data4[1] = position.y;
    data4[2] = position.z;

	data1[0] = std::cos(yaw);
	// data1[1] = 0;
	data1[2] = -std::sin(yaw);

	data2[0] = std::sin(pitch) * std::sin(yaw);
	data2[1] = std::cos(pitch);
	data2[2] = std::sin(pitch) * std::cos(yaw);

	data3[0] = std::cos(pitch) * std::sin(yaw);
	data3[1] = -std::sin(pitch);
	data3[2] = std::cos(pitch) * std::cos(yaw);

    mModelMatrixData1.insert(mModelMatrixData1.end(), data1.begin(), data1.end());
    mModelMatrixData2.insert(mModelMatrixData2.end(), data2.begin(), data2.end());
    mModelMatrixData3.insert(mModelMatrixData3.end(), data3.begin(), data3.end());
    mModelMatrixData4.insert(mModelMatrixData4.end(), data4.begin(), data4.end());
    mColorData.push_back(color.x);
    mColorData.push_back(color.y);
    mColorData.push_back(color.z);

    mRenderAmount++;
}

void DebugRenderer::render(const Camera& camera, const glm::mat4& perspective, const Shader& shader)
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

    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mRenderAmount);
    mRenderAmount = 0;

    mVertexArray.unbind();
}

std::type_index DebugRenderer::getRenderableType() const
{
    return std::type_index(typeid(DebugRenderable));   
}
