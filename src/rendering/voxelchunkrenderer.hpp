#pragma once
#include "rendermodule.hpp"
#include "voxelchunkrenderable.hpp"
#include "opengl.hpp"
#include "vao.hpp"
#include "buffer.hpp"
#include "shader.hpp"

class Mesh;

class VoxelChunkRenderer : public RenderModule
{
    public:
        VoxelChunkRenderer();
        void queue(const Renderable& renderable) override;
        void render(const Camera& camera, const glm::mat4& perspective, const Shader& shader) override;
        std::type_index getRenderableType() const override;
    private:
        VAO mVertexArray;

        std::vector<const Model*> mOrders;

    Buffer mColors;
    Buffer mModelMatrix1;
    Buffer mModelMatrix2;
    Buffer mModelMatrix3;
    Buffer mModelMatrix4;
    Buffer mNormalMatrix1;
    Buffer mNormalMatrix2;
    Buffer mNormalMatrix3;
    Buffer mNormalMatrix4;
};
