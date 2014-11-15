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
    mVertexBuffer.bind();

    mVertexBuffer.setData(vertices);

    std::string vertexSource = R"(
#version 150
attribute vec3 in_position;

void main()
{
    gl_Position = vec4(in_position.x, in_position.y, in_position.z, 1.0);
})";

    std::string fragmentSource = R"(
#version 150
precision highp float;

out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0,1.0,1.0,1.0);
})";

    mShader.setSource(vertexSource, fragmentSource);
    mShader.compile();

    mShader.setVertexAttribute("in_position", 3, 0);

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
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

std::type_index DebugRenderer::getRenderableType() const
{
    return std::type_index(typeid(DebugRenderable));   
}
