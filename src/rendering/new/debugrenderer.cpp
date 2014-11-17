#include "debugrenderer.hpp"
#include "camera.hpp"
#include <vector>
#include <string>
#include <iostream>

DebugRenderer::DebugRenderer()
{
    std::string vertexSource = R"(
#version 150

uniform mat4 viewMatrix;

attribute vec3 in_position;
attribute vec3 offset;
attribute vec3 color;

out vec3 objectColor;

void main()
{
    gl_Position = vec4(vec3(in_position.x, in_position.y, in_position.z) + offset, 1.0) * viewMatrix;
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
             -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
               0.0f,  1.0f, 0.0f,
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

    mPositionData.push_back(position.x);
    mPositionData.push_back(position.y);
    mPositionData.push_back(position.z);
    mColorData.push_back(color.x);
    mColorData.push_back(color.y);
    mColorData.push_back(color.z);
}

void DebugRenderer::render(const Camera& camera)
{
    uint32_t renderAmount = mPositionData.size() / 3;
    mOffsetBuffer.setData(mPositionData);
    mColorBuffer.setData(mColorData);

    mPositionData.clear();
    mColorData.clear();

    mShader.setInstanceAttribute("offset", 3, mOffsetBuffer, 1);
    mShader.setInstanceAttribute("color", 3, mColorBuffer, 1);

    mShader.setUniform("viewMatrix", MAT4X4, glm::value_ptr(camera.getMatrix()));

    mShader.activate();

    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, renderAmount);
}

std::type_index DebugRenderer::getRenderableType() const
{
    return std::type_index(typeid(DebugRenderable));   
}
