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
        void queue(const Renderable& renderable) override;
        void render(const Camera& camera) override;
        std::type_index getRenderableType() const override;
    private:
        VAO mVertexArray;
        Buffer mVertexBuffer;
        Buffer mOffsetBuffer;
        Buffer mColorBuffer;
        Shader mShader;

        GLuint shaderProgram;
        GLuint vertexShader, fragmentShader;

        //frame data
        std::vector<float> mPositionData;
        std::vector<float> mColorData;
};
