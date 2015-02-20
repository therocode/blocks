#include "modelrenderer.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "texturearray.hpp"
#include "shaderattribute.hpp"
#include <vector>
#include <string>
#include <fea/assert.hpp>

ModelBufferStorage::ModelBufferStorage() : 
    colors(Buffer::ARRAY_BUFFER, Buffer::DYNAMIC),
    textureIndices(Buffer::ARRAY_BUFFER, Buffer::DYNAMIC),
    modelMatrix1(Buffer::ARRAY_BUFFER, Buffer::DYNAMIC),
    modelMatrix2(Buffer::ARRAY_BUFFER, Buffer::DYNAMIC),
    modelMatrix3(Buffer::ARRAY_BUFFER, Buffer::DYNAMIC),
    modelMatrix4(Buffer::ARRAY_BUFFER, Buffer::DYNAMIC),
    normalMatrix1(Buffer::ARRAY_BUFFER, Buffer::DYNAMIC),
    normalMatrix2(Buffer::ARRAY_BUFFER, Buffer::DYNAMIC),
    normalMatrix3(Buffer::ARRAY_BUFFER, Buffer::DYNAMIC),
    normalMatrix4(Buffer::ARRAY_BUFFER, Buffer::DYNAMIC),
    animData(Buffer::ARRAY_BUFFER, Buffer::DYNAMIC)
{
}

const int32_t animationUniformBlockSize = 16384;
const int32_t maxBoneAmount = 256;
const int32_t boneMemorySize = 4 * 16;

ModelRenderer::ModelRenderer() :
    mAnimationBlockLocation(-1)
{
}

void ModelRenderer::queue(const Renderable& renderable)
{
    const ModelRenderable& modelRenderable = (const ModelRenderable&) renderable;

    ModelOrder order;
    order.model = modelRenderable.findModel();
    order.textureArray = modelRenderable.findTextureArray();
    order.textureIndex = modelRenderable.getTextureIndex();
    order.color = modelRenderable.getColor();
    order.position = modelRenderable.getPosition();
    order.orientation = modelRenderable.getOrientation();
    order.frameOffset = modelRenderable.getFrameOffset();
    order.animation = modelRenderable.getAnimation();
    
    FEA_ASSERT(order.model != nullptr, "Trying to render a model renderable which doesn't have a model");
    FEA_ASSERT(order.model->findMesh(0) != nullptr, "Trying to render a model renderable which has a model without a primary model");
    FEA_ASSERT(order.model->getJointStructure().size() <= maxBoneAmount, "Trying to render a model with more bones than " + std::to_string(maxBoneAmount) + " is not allowed!");

    mOrders[{order.model, order.textureArray}].push_back(order);
}

void ModelRenderer::render(const Camera& camera, const glm::mat4& perspective, const Shader& shader)
{
    mVertexArray.bind();

    shader.setUniform("viewProjectionMatrix", UniformType::MAT4X4, glm::value_ptr(perspective * camera.getMatrix()));
    float shadedRatio = 1.0f;
    shader.setUniform("shadedRatio", UniformType::FLOAT, &shadedRatio);
    
    for(const auto modelIterator : mOrders)
    {
        const Model& model = *modelIterator.first.first;
        const TextureArray* textureArray = modelIterator.first.second;
        const auto& modelOrders = modelIterator.second;
        int32_t jointAmount = model.getJointStructure().size();
        int32_t maxInstanceAmount = animationUniformBlockSize / (boneMemorySize * jointAmount);

        if(textureArray != nullptr)
        {
            GLuint textureId = textureArray->getId();
            shader.setUniform("textureArray", UniformType::TEXTURE_ARRAY, &textureId);
        }

        shader.setUniform("boneAmount", UniformType::INT, &jointAmount);


        const auto iterator = mModelBufferCache.find(&model);
        if(iterator == mModelBufferCache.end())
        {
            cacheModel(model);
        }

        auto& modelBufferStorage = mModelBufferCache.at(&model);

        modelBufferStorage->vertexArray.bind();

        for(auto iterator = modelIterator.second.begin(); iterator != modelIterator.second.end();)
        {
            int32_t batchSize = std::min(maxInstanceAmount,(int) (modelIterator.second.size() - (iterator - modelIterator.second.begin())));
            uploadBatchData(iterator, batchSize, camera, shader, model, *modelBufferStorage);

            for(const auto& mesh : modelBufferStorage->meshes)
            {
                mesh.bind();
                glDrawElementsInstanced(GL_TRIANGLES, mesh.getElementAmount(), GL_UNSIGNED_INT, 0, batchSize);
            }

            iterator += batchSize;
        }

        modelBufferStorage->vertexArray.unbind();
    }

    mOrders.clear();
}

std::type_index ModelRenderer::getRenderableType() const
{
    return std::type_index(typeid(ModelRenderable));   
}

void ModelRenderer::cacheModel(const Model& model)
{
    std::unique_ptr<ModelBufferStorage> newModelBufferStorage = std::unique_ptr<ModelBufferStorage>(new ModelBufferStorage());

    newModelBufferStorage->vertexArray.bind();

    const auto* positionData = model.findVertexArray(ModelAttribute::POSITIONS);
    FEA_ASSERT(positionData != nullptr, "Cannot render model without position data");
    newModelBufferStorage->modelBuffers.emplace(ModelAttribute::POSITIONS, Buffer(*positionData, Buffer::ARRAY_BUFFER));
    newModelBufferStorage->vertexArray.setVertexAttribute(ShaderAttribute::POSITION, 3, newModelBufferStorage->modelBuffers.at(ModelAttribute::POSITIONS));

    const auto* normalData = model.findVertexArray(ModelAttribute::NORMALS);
    if(normalData != nullptr)
    {
        newModelBufferStorage->modelBuffers.emplace(ModelAttribute::NORMALS, Buffer(*normalData, Buffer::ARRAY_BUFFER));
        newModelBufferStorage->vertexArray.setVertexAttribute(ShaderAttribute::NORMAL, 3, newModelBufferStorage->modelBuffers.at(ModelAttribute::NORMALS));
    }

    const auto* texcoordData = model.findVertexArray(ModelAttribute::TEXCOORDS);
    if(texcoordData != nullptr)
    {
        newModelBufferStorage->modelBuffers.emplace(ModelAttribute::TEXCOORDS, Buffer(*texcoordData, Buffer::ARRAY_BUFFER));
        newModelBufferStorage->vertexArray.setVertexAttribute(ShaderAttribute::TEXCOORD, 2, newModelBufferStorage->modelBuffers.at(ModelAttribute::TEXCOORDS));
    }

    const auto* blendWeightData = model.findBlendArray(ModelAttribute::BLENDWEIGHTS);
    if(blendWeightData != nullptr)
    {
        newModelBufferStorage->modelBuffers.emplace(ModelAttribute::BLENDWEIGHTS, Buffer(*blendWeightData, Buffer::ARRAY_BUFFER));
        newModelBufferStorage->vertexArray.setVertexIntegerAttribute(ShaderAttribute::BLENDWEIGHTS, 4, newModelBufferStorage->modelBuffers.at(ModelAttribute::BLENDWEIGHTS), GL_UNSIGNED_BYTE);
    }

    const auto* blendIndexData = model.findBlendArray(ModelAttribute::BLENDINDICES);
    if(blendIndexData != nullptr)
    {
        newModelBufferStorage->modelBuffers.emplace(ModelAttribute::BLENDINDICES, Buffer(*blendIndexData, Buffer::ARRAY_BUFFER));
        newModelBufferStorage->vertexArray.setVertexIntegerAttribute(ShaderAttribute::BLENDINDICES, 4, newModelBufferStorage->modelBuffers.at(ModelAttribute::BLENDINDICES), GL_UNSIGNED_BYTE);
    }

    newModelBufferStorage->vertexArray.setInstanceAttribute(ShaderAttribute::COLOR, 3, newModelBufferStorage->colors, 1);
    newModelBufferStorage->vertexArray.setInstanceIntegerAttribute(ShaderAttribute::TEXTUREINDEX, 1, newModelBufferStorage->textureIndices, 1, GL_UNSIGNED_INT);
    newModelBufferStorage->vertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX1, 4, newModelBufferStorage->modelMatrix1, 1);
    newModelBufferStorage->vertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX2, 4, newModelBufferStorage->modelMatrix2, 1);
    newModelBufferStorage->vertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX3, 4, newModelBufferStorage->modelMatrix3, 1);
    newModelBufferStorage->vertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX4, 4, newModelBufferStorage->modelMatrix4, 1);
    newModelBufferStorage->vertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX1, 4, newModelBufferStorage->normalMatrix1, 1);
    newModelBufferStorage->vertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX2, 4, newModelBufferStorage->normalMatrix2, 1);
    newModelBufferStorage->vertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX3, 4, newModelBufferStorage->normalMatrix3, 1);
    newModelBufferStorage->vertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX4, 4, newModelBufferStorage->normalMatrix4, 1);

    for(const auto& mesh : model.getMeshes())
    {
        newModelBufferStorage->meshes.push_back({mesh.second->getIndices(), Buffer::ELEMENT_ARRAY_BUFFER });
    }

    mModelBufferCache.emplace(&model, std::move(newModelBufferStorage));
}

void ModelRenderer::uploadBatchData(std::vector<ModelOrder>::const_iterator startOrder, int32_t orderAmount, const Camera& camera, const Shader& shader, const Model& model, ModelBufferStorage& modelBufferStorage)
{
    mColors.resize(orderAmount * 3);
    std::vector<uint32_t> textureIndices;
    std::vector<float> modelMatrix1;
    std::vector<float> modelMatrix2;
    std::vector<float> modelMatrix3;
    std::vector<float> modelMatrix4;
    std::vector<float> normalMatrix1;
    std::vector<float> normalMatrix2;
    std::vector<float> normalMatrix3;
    std::vector<float> normalMatrix4;

    std::vector<float> data1 = { 1.0f, 0.0f, 0.0f, 0.0f };
    std::vector<float> data2 = { 0.0f, 1.0f, 0.0f, 0.0f };
    std::vector<float> data3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    std::vector<float> data4 = { 0.0f, 0.0f, 0.0f, 1.0f };

    std::vector<float> nData1 = { 1.0f, 0.0f, 0.0f, 0.0f };
    std::vector<float> nData2 = { 0.0f, 1.0f, 0.0f, 0.0f };
    std::vector<float> nData3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    std::vector<float> nData4 = { 0.0f, 0.0f, 0.0f, 1.0f };

    for(int32_t orderIndex = 0; orderIndex < orderAmount; orderIndex++)
    {
        int32_t colorIndex = orderIndex * 3;
        const auto& order = *(startOrder + orderIndex);
        mColors[colorIndex    ] = (order.color.r);
        mColors[colorIndex + 1] = (order.color.g);
        mColors[colorIndex + 2] = (order.color.b);

        textureIndices.push_back(order.textureIndex);

        data4[0] = order.position.x;
        data4[1] = order.position.y;
        data4[2] = order.position.z;

        glm::mat3 orientationMat = glm::mat3_cast(order.orientation);
        data1[0] = orientationMat[0][0];
        data1[1] = orientationMat[1][0];
        data1[2] = orientationMat[2][0];
        data2[0] = orientationMat[0][1];
        data2[1] = orientationMat[1][1];
        data2[2] = orientationMat[2][1];
        data3[0] = orientationMat[0][2];
        data3[1] = orientationMat[1][2];
        data3[2] = orientationMat[2][2];

        modelMatrix1.insert(modelMatrix1.end(), data1.begin(), data1.end());
        modelMatrix2.insert(modelMatrix2.end(), data2.begin(), data2.end());
        modelMatrix3.insert(modelMatrix3.end(), data3.begin(), data3.end());
        modelMatrix4.insert(modelMatrix4.end(), data4.begin(), data4.end());

        //do rotation

        glm::mat4 modelMatrix;

        modelMatrix[0][0] = modelMatrix1[0];
        modelMatrix[0][1] = modelMatrix1[1];
        modelMatrix[0][2] = modelMatrix1[2];
        modelMatrix[0][3] = modelMatrix1[3];
        modelMatrix[1][0] = modelMatrix2[0];
        modelMatrix[1][1] = modelMatrix2[1];
        modelMatrix[1][2] = modelMatrix2[2];
        modelMatrix[1][3] = modelMatrix2[3];
        modelMatrix[2][0] = modelMatrix3[0];
        modelMatrix[2][1] = modelMatrix3[1];
        modelMatrix[2][2] = modelMatrix3[2];
        modelMatrix[2][3] = modelMatrix3[3];
        modelMatrix[3][0] = modelMatrix4[0];
        modelMatrix[3][1] = modelMatrix4[1];
        modelMatrix[3][2] = modelMatrix4[2];
        modelMatrix[3][3] = modelMatrix4[3];

        glm::mat4 normalMatrix = glm::transpose(glm::inverse(camera.getMatrix() * modelMatrix));

        nData1[0] = normalMatrix[0][0];
        nData1[1] = normalMatrix[0][1];
        nData1[2] = normalMatrix[0][2];
        nData1[3] = normalMatrix[0][3];
        nData2[0] = normalMatrix[1][0];
        nData2[1] = normalMatrix[1][1];
        nData2[2] = normalMatrix[1][2];
        nData2[3] = normalMatrix[1][3];
        nData3[0] = normalMatrix[2][0];
        nData3[1] = normalMatrix[2][1];
        nData3[2] = normalMatrix[2][2];
        nData3[3] = normalMatrix[2][3];
        nData4[0] = normalMatrix[3][0];
        nData4[1] = normalMatrix[3][1];
        nData4[2] = normalMatrix[3][2];
        nData4[3] = normalMatrix[3][3];

        normalMatrix1.insert(normalMatrix1.end(), nData1.begin(), nData1.end());
        normalMatrix2.insert(normalMatrix2.end(), nData2.begin(), nData2.end());
        normalMatrix3.insert(normalMatrix3.end(), nData3.begin(), nData3.end());
        normalMatrix4.insert(normalMatrix4.end(), nData4.begin(), nData4.end());
    }

    const auto& jointStructure = model.getJointStructure();
    int32_t numJoints = jointStructure.size();

    //animation
    std::vector<float> rotData(12 * numJoints);
    std::vector<float> transData(4 * numJoints);
    mTotalRotData.resize(12 * numJoints * orderAmount);
    mTotalTransData.resize(4 * numJoints * orderAmount);

    for(int32_t orderIndex = 0; orderIndex < orderAmount; orderIndex++)
    {
        const auto& order = *(startOrder + orderIndex);
        const Animation* animation = model.findAnimation(order.animation);

        if(animation != nullptr)
        {
            float localCurrentFrame = (order.frameOffset / 60.0f) * animation->framerate;
            int32_t numFrames = animation->frameAmount;
            int32_t frame1 = (int32_t)std::floor(localCurrentFrame);// * (animation->framerate/60.0f));
            int32_t frame2 = frame1 + 1;
            float frameOffset = localCurrentFrame- frame1;
            frame1 %= numFrames;
            frame2 %= numFrames;

            const auto rotation1 = animation->rotations.begin() + frame1 * numJoints;
            const auto rotation2 = animation->rotations.begin() + frame2 * numJoints;
            const auto translation1 = animation->translations.begin() + frame1 * numJoints;
            const auto translation2 = animation->translations.begin() + frame2 * numJoints;

            std::vector<glm::mat4x4> outputTransformation(numJoints);

            for(int32_t i = 0; i < numJoints; i++)
            {
                int32_t rotIndex = i * 12;
                int32_t transIndex = i * 4;
                glm::mat3x3 rotation  = rotation1[i]    * (1.0f - frameOffset) + rotation2[i]    * frameOffset;
                glm::vec3 translation = translation1[i] * (1.0f - frameOffset) + translation2[i] * frameOffset;

                glm::mat4x4 transformation = glm::translate(glm::mat4x4(rotation), translation);
                transformation[3][0] = translation.x;
                transformation[3][1] = translation.y;
                transformation[3][2] = translation.z;

                FEA_ASSERT(i > jointStructure[i], "Parent structure messed up. Joint " + std::to_string(i) + " has the parent " + std::to_string(jointStructure[i]) + ".");
                if(jointStructure[i] >= 0)
                {
                    outputTransformation[i] = outputTransformation[jointStructure[i]] * transformation;
                }
                else
                {
                    outputTransformation[i] = transformation;
                }

                const float* floatIter = glm::value_ptr(outputTransformation[i]);
                rotData[rotIndex + 0]  = *floatIter;
                rotData[rotIndex + 1]  = *(floatIter + 1);
                rotData[rotIndex + 2]  = *(floatIter + 2);
                rotData[rotIndex + 3]  = 1337.0f;//skipped due to padding
                rotData[rotIndex + 4]  = *(floatIter + 4);
                rotData[rotIndex + 5]  = *(floatIter + 5);
                rotData[rotIndex + 6]  = *(floatIter + 6);
                rotData[rotIndex + 7]  = 1337.1f;//skipped due to padding
                rotData[rotIndex + 8]  = *(floatIter + 8);
                rotData[rotIndex + 9]  = *(floatIter + 9);
                rotData[rotIndex + 10] = *(floatIter + 10);
                rotData[rotIndex + 11] = 1337.2f; //skipped due to padding
                transData[transIndex + 0] = outputTransformation[i][3][0];
                transData[transIndex + 1] = outputTransformation[i][3][1];
                transData[transIndex + 2] = outputTransformation[i][3][2];
                transData[transIndex + 3] = 1337.3f;//skipped due to padding
            }
        }
        else
        {
            for(int32_t i = 0; i < numJoints; i++)
            {
                int32_t rotIndex = i * 12;
                int32_t transIndex = i * 4;
                rotData[rotIndex + 0]  = 1.0f;
                rotData[rotIndex + 1]  = 0.0f;
                rotData[rotIndex + 2]  = 0.0f;
                rotData[rotIndex + 3]  = 1337.0f;//skipped due to padding
                rotData[rotIndex + 4]  = 0.0f;
                rotData[rotIndex + 5]  = 1.0f;
                rotData[rotIndex + 6]  = 0.0f;
                rotData[rotIndex + 7]  = 1337.1f;//skipped due to padding
                rotData[rotIndex + 8]  = 0.0f;
                rotData[rotIndex + 9]  = 0.0f;
                rotData[rotIndex + 10] = 1.0f;
                rotData[rotIndex + 11] = 1337.2f; //skipped due to padding
                transData[transIndex + 0] = 0.0f;
                transData[transIndex + 1] = 0.0f;
                transData[transIndex + 2] = 0.0f;
                transData[transIndex + 3] = 1337.3f;//skipped due to padding
            }
        }
        std::copy(rotData.begin(), rotData.end(), mTotalRotData.begin() + orderIndex * 12 * numJoints);
        std::copy(transData.begin(), transData.end(), mTotalTransData.begin() + orderIndex * 4 * numJoints);
    }
    //animation end

    mTotalAnimData.resize(animationUniformBlockSize);
    std::copy(mTotalRotData.begin(), mTotalRotData.end(), mTotalAnimData.begin());
    std::copy(mTotalTransData.begin(), mTotalTransData.end(), mTotalAnimData.begin() + maxBoneAmount * 12);

    modelBufferStorage.colors.setData(mColors);
    modelBufferStorage.textureIndices.setData(textureIndices);
    modelBufferStorage.modelMatrix1.setData(modelMatrix1);
    modelBufferStorage.modelMatrix2.setData(modelMatrix2);
    modelBufferStorage.modelMatrix3.setData(modelMatrix3);
    modelBufferStorage.modelMatrix4.setData(modelMatrix4);
    modelBufferStorage.normalMatrix1.setData(normalMatrix1);
    modelBufferStorage.normalMatrix2.setData(normalMatrix2);
    modelBufferStorage.normalMatrix3.setData(normalMatrix3);

    modelBufferStorage.animData.setData(mTotalAnimData);

    modelBufferStorage.vertexArray.bind();

    int32_t blockIndex = 0;

    if(mAnimationBlockLocation == -1)
        mAnimationBlockLocation = glGetUniformBlockIndex(shader.getId(), "AnimationBlock");

    glUniformBlockBinding(shader.getId(), mAnimationBlockLocation, blockIndex);
    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, modelBufferStorage.animData.getId());
}
