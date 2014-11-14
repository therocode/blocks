#pragma once
#include "rendermodule.hpp"
#include "debugrenderable.hpp"
#include "../opengl.hpp"
#include "vao.hpp"
#include "buffer.hpp"

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

        GLuint shaderProgram;
        GLuint vertexShader, fragmentShader;
};
