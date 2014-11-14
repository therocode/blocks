#pragma once
#include "rendermodule.hpp"
#include "debugrenderable.hpp"
#include "../opengl.hpp"
#include "buffer.hpp"

class DebugRenderer : public RenderModule
{
    public:
        DebugRenderer();
        virtual void queue(const Renderable& renderable);
        virtual void render();
        virtual std::type_index getRenderableType() const;
    private:
        Buffer mVertexBuffer;
        GLuint mVertexArray;

        GLuint shaderProgram;
        GLuint vertexShader, fragmentShader;
};
