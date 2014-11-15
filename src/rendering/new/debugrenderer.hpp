#pragma once
#include "rendermodule.hpp"
#include "debugrenderable.hpp"
#include "../opengl.hpp"
#include "vao.hpp"
#include "buffer.hpp"
#include "shader.hpp"

class DebugRenderer : public RenderModule
{
    public:
        DebugRenderer();
        virtual void queue(const Renderable& renderable);
        virtual void render();
        virtual std::type_index getRenderableType() const;
    private:
        VAO mVertexArray;
        Buffer mVertexBuffer;
        Shader mShader;

        GLuint shaderProgram;
        GLuint vertexShader, fragmentShader;
};
