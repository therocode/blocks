#include "voxelchunkrenderer.hpp"
#include "camera.hpp"
#include "chunkmodel.hpp"
#include "shaderattribute.hpp"
#include <vector>
#include <string>
#include <fea/assert.hpp>

CachedChunk::CachedChunk() :
    positionBuffer(GL_ARRAY_BUFFER),
    normalBuffer(GL_ARRAY_BUFFER),
    texCoordBuffer(GL_ARRAY_BUFFER),
    textureIndicesBuffer(GL_ARRAY_BUFFER)
{
}

VoxelChunkRenderer::VoxelChunkRenderer() :
    mColors(Buffer::ARRAY_BUFFER),
    mTextureIndices(Buffer::ARRAY_BUFFER),
    mModelMatrix1(Buffer::ARRAY_BUFFER),
    mModelMatrix2(Buffer::ARRAY_BUFFER),
    mModelMatrix3(Buffer::ARRAY_BUFFER),
    mModelMatrix4(Buffer::ARRAY_BUFFER),
    mNormalMatrix1(Buffer::ARRAY_BUFFER),
    mNormalMatrix2(Buffer::ARRAY_BUFFER),
    mNormalMatrix3(Buffer::ARRAY_BUFFER),
    mNormalMatrix4(Buffer::ARRAY_BUFFER),
    mCurrentTextureArray(nullptr)
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
    const VoxelChunkRenderable& voxelChunkRenderable = (const VoxelChunkRenderable&) renderable;

    if(voxelChunkRenderable.findModel() != nullptr)
        mOrders.push_back(voxelChunkRenderable);

    mCurrentTextureArray = voxelChunkRenderable.findTextureArray();
}

void VoxelChunkRenderer::render(const Camera& camera, const glm::mat4& perspective, const Shader& shader)
{
    mVertexArray.bind();

    if(mCurrentTextureArray)
    {
        GLuint textureId = mCurrentTextureArray->getId();
        shader.setUniform("textureArray", UniformType::TEXTURE_ARRAY, &textureId);
    }

    shader.setUniform("viewProjectionMatrix", UniformType::MAT4X4, glm::value_ptr(perspective * camera.getMatrix()));
    float shadedRatio = 1.0f;
    shader.setUniform("shadedRatio", UniformType::FLOAT, &shadedRatio);
    
    for(const auto order : mOrders)
    {
        uint32_t renderAmount = order.findModel()->model.findVertexArray(ModelAttribute::POSITIONS)->size() / 3;

        if(renderAmount > 0)
        {
            Buffer positionBuffer(*order.findModel()->model.findVertexArray(ModelAttribute::POSITIONS), Buffer::ARRAY_BUFFER);
            Buffer normalBuffer(*order.findModel()->model.findVertexArray(ModelAttribute::NORMALS), Buffer::ARRAY_BUFFER);
            Buffer texCoordBuffer(*order.findModel()->model.findVertexArray(ModelAttribute::TEXCOORDS), Buffer::ARRAY_BUFFER);
            Buffer textureIndicesBuffer(order.findModel()->textureIndices, Buffer::ARRAY_BUFFER);

            mVertexArray.setVertexAttribute(ShaderAttribute::POSITION, 3, positionBuffer);
            mVertexArray.setVertexAttribute(ShaderAttribute::NORMAL, 3, normalBuffer);
            mVertexArray.setVertexAttribute(ShaderAttribute::TEXCOORD, 2, texCoordBuffer);
            mVertexArray.setVertexIntegerAttribute(ShaderAttribute::TEXTUREINDEX, 1, textureIndicesBuffer, GL_UNSIGNED_INT);

            mVertexArray.setInstanceAttribute(ShaderAttribute::COLOR, 3, mColors, 1);
            mVertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX1, 4, mModelMatrix1, 1);
            mVertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX2, 4, mModelMatrix2, 1);
            mVertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX3, 4, mModelMatrix3, 1);
            mVertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX4, 4, mModelMatrix4, 1);
            mVertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX1, 4, mNormalMatrix1, 1);
            mVertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX2, 4, mNormalMatrix2, 1);
            mVertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX3, 4, mNormalMatrix3, 1);
            mVertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX4, 4, mNormalMatrix4, 1);

            glDrawArraysInstanced(GL_TRIANGLES, 0, renderAmount, 1);
        }
    }

    mOrders.clear();

    mVertexArray.unbind();
}

std::type_index VoxelChunkRenderer::getRenderableType() const
{
    return std::type_index(typeid(VoxelChunkRenderable));   
}
