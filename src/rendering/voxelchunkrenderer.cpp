#include "voxelchunkrenderer.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "shaderattribute.hpp"
#include <vector>
#include <string>
#include <fea/assert.hpp>

VoxelChunkRenderer::VoxelChunkRenderer()
{
    mColors.setData(std::vector<float>({0.3f, 0.55555f,.0f}));
    mModelMatrix1.setData(std::vector<float>({1.0f, 0.0f, 0.0f, 0.0f }));
    mModelMatrix2.setData(std::vector<float>({0.0f, 1.0f, 0.0f, 0.0f }));
    mModelMatrix3.setData(std::vector<float>({0.0f, 0.0f, 1.0f, 0.0f }));
    mModelMatrix4.setData(std::vector<float>({0.0f, 0.0f, 0.0f, 1.0f }));
    mNormalMatrix1.setData(std::vector<float>({1.0f, 0.0f, 0.0f, 0.0f }));
    mNormalMatrix2.setData(std::vector<float>({0.0f, 1.0f, 0.0f, 0.0f }));
    mNormalMatrix3.setData(std::vector<float>({0.0f, 0.0f, 1.0f, 0.0f }));
    mNormalMatrix4.setData(std::vector<float>({0.0f, 0.0f, 0.0f, 1.0f }));
}

void VoxelChunkRenderer::queue(const Renderable& renderable)
{
    const VoxelChunkRenderable& modelRenderable = (const VoxelChunkRenderable&) renderable;

    const Model* model = modelRenderable.findModel();
    
    if(model != nullptr)
        mOrders.push_back(model);
}

void VoxelChunkRenderer::render(const Camera& camera, const glm::mat4& perspective, const Shader& shader)
{
    mVertexArray.bind();

    shader.setUniform("viewProjectionMatrix", UniformType::MAT4X4, glm::value_ptr(perspective * camera.getMatrix()));
    float shadedRatio = 0.5f;
    shader.setUniform("shadedRatio", UniformType::FLOAT, &shadedRatio);
    
    for(const auto model : mOrders)
    {
        mVertexArray.setVertexAttribute(ShaderAttribute::POSITION, 3, *model->findVertexArray(Model::POSITIONS));
        mVertexArray.setVertexAttribute(ShaderAttribute::NORMAL, 3, *model->findVertexArray(Model::NORMALS));

        mVertexArray.setInstanceAttribute(ShaderAttribute::COLOR, 3, mColors, 1);
        mVertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX1, 4, mModelMatrix1, 1);
        mVertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX2, 4, mModelMatrix2, 1);
        mVertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX3, 4, mModelMatrix3, 1);
        mVertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX4, 4, mModelMatrix4, 1);
        mVertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX1, 4, mNormalMatrix1, 1);
        mVertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX2, 4, mNormalMatrix2, 1);
        mVertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX3, 4, mNormalMatrix3, 1);
        mVertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX4, 4, mNormalMatrix4, 1);

        glDrawArraysInstanced(GL_TRIANGLES, 0, model->findVertexArray(Model::POSITIONS)->getElementAmount() / 3, 1);
    }

    mOrders.clear();
}

std::type_index VoxelChunkRenderer::getRenderableType() const
{
    return std::type_index(typeid(VoxelChunkRenderable));   
}
