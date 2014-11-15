#include "debugrenderer.hpp"
#include <vector>
#include <string>
#include <iostream>

DebugRenderer::DebugRenderer()
{
    std::vector<float> vertices = {
        //-1.0f,  1.0f, -1.0f,
        //-1.0f, -1.0f, -1.0f,
        // 1.0f, -1.0f, -1.0f
             -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
               0.0f,  1.0f, 0.0f,
    };

    mVertexArray.bind();
    mVertexBuffer.setData(vertices);

    std::vector<float> positions(10 * 3);
    for(uint32_t i = 0; i < positions.size() / 3; i++)
    {
        float factor = (float)i / 10.0f;
        positions[i * 3    ] = factor - 1.0f;
        positions[i * 3 + 1] = factor;
        positions[i * 3 + 2] = factor;
    }

    mOffsetBuffer.setData(positions);

    std::string vertexSource = R"(
#version 150

attribute vec3 in_position;
attribute vec3 offset;
flat out int instanceId;

void main()
{
    gl_Position = vec4(vec3(in_position.x, in_position.y, in_position.z) + offset, 1.0);
    instanceId = gl_InstanceID;
})";

    std::string fragmentSource = R"(
#version 150
precision highp float;

out vec4 fragColor;
flat in int instanceId;

void main()
{
    fragColor = vec4(1.0 * (float(instanceId) * 0.2f),1.0,1.0,1.0);
})";

    mShader.setSource(vertexSource, fragmentSource);
    mShader.compile();

    mShader.setVertexAttribute("in_position", 3, mVertexBuffer);
    mShader.setInstanceAttribute("offset", 3, mOffsetBuffer, 1);

    std::cout << "error: " << gluErrorString(glGetError()) << "\n";
}

void DebugRenderer::queue(const Renderable& renderable)
{
}

void DebugRenderer::render()
{
    mShader.activate();
    mVertexArray.bind();
    mVertexBuffer.bind();


    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 10);
}

std::type_index DebugRenderer::getRenderableType() const
{
    return std::type_index(typeid(DebugRenderable));   
}
