#include "modelrenderer.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "texturearray.hpp"
#include "shaderattribute.hpp"
#include <vector>
#include <string>
#include <fea/assert.hpp>

ModelRenderer::ModelRenderer() : 
    mCurFrame(0.0f)
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
    
    FEA_ASSERT(order.model != nullptr, "Trying to render a model renderable which doesn't have a model");
    FEA_ASSERT(order.model->findMesh(0) != nullptr, "Trying to render a model renderable which has a model without a primary model");

    mOrders[order.model].push_back(order);
}

void ModelRenderer::render(const Camera& camera, const glm::mat4& perspective, const Shader& shader)
{
    mVertexArray.bind();

    shader.setUniform("viewProjectionMatrix", UniformType::MAT4X4, glm::value_ptr(perspective * camera.getMatrix()));
    float shadedRatio = 1.0f;
    shader.setUniform("shadedRatio", UniformType::FLOAT, &shadedRatio);
    
    for(const auto modelIterator : mOrders)
    {
        const Model& model = *modelIterator.first;


        const auto iterator = mModelCache.find(&model);
        if(iterator == mModelCache.end())
        {
            std::unique_ptr<ModelObject> newModelObject = std::unique_ptr<ModelObject>(new ModelObject());

            newModelObject->vertexArray.setVertexAttribute(ShaderAttribute::POSITION, 3, *model.findVertexArray(Model::POSITIONS));
            newModelObject->vertexArray.setVertexAttribute(ShaderAttribute::NORMAL, 3, *model.findVertexArray(Model::NORMALS));
            newModelObject->vertexArray.setVertexAttribute(ShaderAttribute::TEXCOORD, 2, *model.findVertexArray(Model::TEXCOORDS));
            newModelObject->vertexArray.setVertexIntegerAttribute(ShaderAttribute::BLENDWEIGHTS, 4, *model.findVertexArray(Model::BLENDWEIGHTS), GL_UNSIGNED_BYTE);
            newModelObject->vertexArray.setVertexIntegerAttribute(ShaderAttribute::BLENDINDICES, 4, *model.findVertexArray(Model::BLENDINDICES), GL_UNSIGNED_BYTE);

            newModelObject->vertexArray.setInstanceAttribute(ShaderAttribute::COLOR, 3, newModelObject->colors, 1);
            newModelObject->vertexArray.setInstanceIntegerAttribute(ShaderAttribute::TEXTUREINDEX, 1, newModelObject->textureIndices, 1, GL_UNSIGNED_INT);
            newModelObject->vertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX1, 4, newModelObject->modelMatrix1, 1);
            newModelObject->vertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX2, 4, newModelObject->modelMatrix2, 1);
            newModelObject->vertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX3, 4, newModelObject->modelMatrix3, 1);
            newModelObject->vertexArray.setInstanceAttribute(ShaderAttribute::MODELMATRIX4, 4, newModelObject->modelMatrix4, 1);
            newModelObject->vertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX1, 4, newModelObject->normalMatrix1, 1);
            newModelObject->vertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX2, 4, newModelObject->normalMatrix2, 1);
            newModelObject->vertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX3, 4, newModelObject->normalMatrix3, 1);
            newModelObject->vertexArray.setInstanceAttribute(ShaderAttribute::NORMALMATRIX4, 4, newModelObject->normalMatrix4, 1);

            for(const auto& mesh : model.getMeshes())
            {
                newModelObject->meshes.push_back(mesh.second.get());
            }

            newModelObject->animation = model.getAnimation();

            mModelCache.emplace(&model, std::move(newModelObject));
        }


        const auto& modelObject = mModelCache.at(&model);

        std::vector<float> colors;
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
        
        for(const auto order : modelIterator.second)
        {
            colors.push_back(order.color.r);
            colors.push_back(order.color.g);
            colors.push_back(order.color.b);

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

            if(order.textureArray != nullptr)
            {
                GLuint textureId = order.textureArray->getId();
                shader.setUniform("textureArray", UniformType::TEXTURE_ARRAY, &textureId);
            }
        }

        int32_t numJoints = model.getJointStructure().size();

        //animation
        std::vector<float> rotData(12 * 128);
        std::vector<float> transData(4 * 128);
        const Animation* animation = model.getAnimation();
        std::vector<float> animationData;
        if(animation != nullptr)
        {
            float localCurrentFrame = (mCurFrame / 60.0f) * animation->framerate;
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


            const auto& jointStructure = model.getJointStructure();

            //std::vector<Matrix3x4> outputTransformation(numJoints);
            std::vector<glm::mat4x4> outputTransformation(numJoints);

            for(int32_t i = 0; i < numJoints; i++)
            {
                int32_t rotIndex = i * 12;
                int32_t transIndex = i * 4;
                glm::mat3x3 rotation    = rotation1[i]    * (1.0f - frameOffset) + rotation2[i]    * frameOffset;
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
                //rotData[rotIndex + 3]  //skipped due to padding
                rotData[rotIndex + 4]  = *(floatIter + 4);
                rotData[rotIndex + 5]  = *(floatIter + 5);
                rotData[rotIndex + 6]  = *(floatIter + 6);
                //rotData[rotIndex + 7]  //skipped due to padding
                rotData[rotIndex + 8]  = *(floatIter + 8);
                rotData[rotIndex + 9]  = *(floatIter + 9);
                rotData[rotIndex + 10] = *(floatIter + 10);
                //rotData[rotIndex + 11]  //skipped due to padding
                transData[transIndex + 0] = outputTransformation[i][3][0];
                transData[transIndex + 1] = outputTransformation[i][3][1];
                transData[transIndex + 2] = outputTransformation[i][3][2];
            }
            animationData.insert(animationData.end(), rotData.begin(), rotData.end());
            animationData.insert(animationData.end(), transData.begin(), transData.end());
        }
        //animation end
        
        modelObject->colors.setData(colors);
        modelObject->textureIndices.setData(textureIndices);
        modelObject->modelMatrix1.setData(modelMatrix1);
        modelObject->modelMatrix2.setData(modelMatrix2);
        modelObject->modelMatrix3.setData(modelMatrix3);
        modelObject->modelMatrix4.setData(modelMatrix4);
        modelObject->normalMatrix1.setData(normalMatrix1);
        modelObject->normalMatrix2.setData(normalMatrix2);
        modelObject->normalMatrix3.setData(normalMatrix3);

        modelObject->animData.setData(animationData);

        modelObject->vertexArray.bind();

        int32_t blockIndex = 0;

        int32_t location = glGetUniformBlockIndex(shader.getId(), "AnimationBlock");
        glUniformBlockBinding(shader.getId(), location, blockIndex);
        glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, modelObject->animData.getId());

        for(const auto& mesh : modelObject->meshes)
        {
            mesh->getIndexBuffer().bind();
            glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndexBuffer().getElementAmount(), GL_UNSIGNED_INT, 0, modelIterator.second.size());
        }

        modelObject->vertexArray.unbind();
    }

    mOrders.clear();

    mCurFrame += 1.0f;
}

std::type_index ModelRenderer::getRenderableType() const
{
    return std::type_index(typeid(ModelRenderable));   
}
