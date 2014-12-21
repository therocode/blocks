#pragma once
#include "rendermodule.hpp"
#include "debugrenderable.hpp"
#include "opengl.hpp"
#include "vao.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "texturearray.hpp"


class DebugRenderer : public RenderModule
{
    public:
        DebugRenderer();
        void queue(const Renderable& renderable) override;
        void render(const Camera& camera, const glm::mat4& perspective, const Shader& shader) override;
        std::type_index getRenderableType() const override;
    private:
        VAO mVertexArray;
        Buffer mVertexBuffer;
        Buffer mModelMatrixBuffer1;
        Buffer mModelMatrixBuffer2;
        Buffer mModelMatrixBuffer3;
        Buffer mModelMatrixBuffer4;
        Buffer mColorBuffer;

		VAO mLineVertexArray;
		Buffer mLineVertexBuffer;
        Buffer mLineColorBuffer;

        //frame data
        std::vector<float> mModelMatrixData1;
        std::vector<float> mModelMatrixData2;
        std::vector<float> mModelMatrixData3;
        std::vector<float> mModelMatrixData4;

        std::vector<float> mColorData;

        uint32_t mRenderAmount;

		std::vector<float> mLineCoords;
		std::vector<float> mLineColorData;

        std::vector<float> data1;
        std::vector<float> data2;
        std::vector<float> data3;
        std::vector<float> data4;

        TextureArray mWhiteTextureArray;
};
