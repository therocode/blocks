#include "modelrenderer.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "model.hpp"
#include <vector>
#include <string>
#include <fea/assert.hpp>

ModelRenderer::ModelRenderer()
{
    mVertexArray.bind();
    std::string vertexSource = R"(
#version 330

uniform mat4 viewProjectionMatrix;

layout(location = ~POSITION~) in vec3 in_position;
layout(location = ~COLOR~) in vec3 color;
layout(location = ~MODELMATRIX1~) in vec4 modelMatrix1;
layout(location = ~MODELMATRIX2~) in vec4 modelMatrix2;
layout(location = ~MODELMATRIX3~) in vec4 modelMatrix3;
layout(location = ~MODELMATRIX4~) in vec4 modelMatrix4;

out vec3 objectColor;

void main()
{
    mat4 modelMatrix = mat4( modelMatrix1, modelMatrix2, modelMatrix3, modelMatrix4 );
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(vec3(in_position.x, in_position.y, in_position.z), 1.0);
    objectColor = color;
})";

    std::string fragmentSource = R"(
#version 330
precision highp float;

in vec3 objectColor;
out vec4 fragColor;

void main()
{
    fragColor = vec4(objectColor, 1.0f);
})";

    mShader.setSource(vertexSource, fragmentSource);
    mShader.compile({
            {"POSITION", ModelAttribute::POSITION},
            {"COLOR", ModelAttribute::COLOR},
            {"MODELMATRIX1", ModelAttribute::MODELMATRIX1},
            {"MODELMATRIX2", ModelAttribute::MODELMATRIX2},
            {"MODELMATRIX3", ModelAttribute::MODELMATRIX3},
            {"MODELMATRIX4", ModelAttribute::MODELMATRIX4}
            });

    mVertexArray.unbind();
}

void ModelRenderer::queue(const Renderable& renderable)
{
    const ModelRenderable& modelRenderable = (const ModelRenderable&) renderable;

    ModelOrder order;
    order.model = modelRenderable.findModel();
    order.color = modelRenderable.getColor();
    order.position = modelRenderable.getPosition();
    
    FEA_ASSERT(order.model != nullptr, "Trying to render a model renderable which doesn't have a model");
    FEA_ASSERT(order.model->findMesh(0) != nullptr, "Trying to render a model renderable which has a model without a primary model");

    mOrders[order.model].push_back(order);
}

void ModelRenderer::render(const Camera& camera, const glm::mat4& perspective)
{
    mVertexArray.bind();
    mShader.activate();

    mShader.setUniform("viewProjectionMatrix", UniformType::MAT4X4, glm::value_ptr(perspective * camera.getMatrix()));
    
    for(const auto modelIterator : mOrders)
    {
        const Model& model = *modelIterator.first;

        const auto iterator = mModelCache.find(&model);
        if(iterator == mModelCache.end())
        {
            std::unique_ptr<ModelObject> newModelObject = std::unique_ptr<ModelObject>(new ModelObject());

            newModelObject->vertexArray.setVertexAttribute(ModelAttribute::POSITION, 3, *model.findVertexArray(Model::POSITIONS));

            newModelObject->vertexArray.setInstanceAttribute(ModelAttribute::COLOR, 3, newModelObject->colors, 1);
            newModelObject->vertexArray.setInstanceAttribute(ModelAttribute::MODELMATRIX1, 4, newModelObject->modelMatrix1, 1);
            newModelObject->vertexArray.setInstanceAttribute(ModelAttribute::MODELMATRIX2, 4, newModelObject->modelMatrix2, 1);
            newModelObject->vertexArray.setInstanceAttribute(ModelAttribute::MODELMATRIX3, 4, newModelObject->modelMatrix3, 1);
            newModelObject->vertexArray.setInstanceAttribute(ModelAttribute::MODELMATRIX4, 4, newModelObject->modelMatrix4, 1);

            for(const auto& mesh : model.getMeshes())
            {
                newModelObject->meshes.push_back(mesh.second.get());
            }

            mModelCache.emplace(&model, std::move(newModelObject));
        }


        const auto& modelObject = mModelCache.at(&model);

        std::vector<float> colors;
        std::vector<float> modelMatrix1;
        std::vector<float> modelMatrix2;
        std::vector<float> modelMatrix3;
        std::vector<float> modelMatrix4;

        std::vector<float> data1 = { 1.0f, 0.0f, 0.0f, 0.0f };
        std::vector<float> data2 = { 0.0f, 1.0f, 0.0f, 0.0f };
        std::vector<float> data3 = { 0.0f, 0.0f, 1.0f, 0.0f };
        std::vector<float> data4 = { 0.0f, 0.0f, 0.0f, 1.0f };
        
        for(const auto order : modelIterator.second)
        {
            colors.push_back(order.color.r);
            colors.push_back(order.color.g);
            colors.push_back(order.color.b);

            data4[0] = order.position.x;
            data4[1] = order.position.y;
            data4[2] = order.position.z;

            modelMatrix1.insert(modelMatrix1.end(), data1.begin(), data1.end());
            modelMatrix2.insert(modelMatrix2.end(), data2.begin(), data2.end());
            modelMatrix3.insert(modelMatrix3.end(), data3.begin(), data3.end());
            modelMatrix4.insert(modelMatrix4.end(), data4.begin(), data4.end());
        }

        modelObject->colors.setData(colors);
        modelObject->modelMatrix1.setData(modelMatrix1);
        modelObject->modelMatrix2.setData(modelMatrix2);
        modelObject->modelMatrix3.setData(modelMatrix3);
        modelObject->modelMatrix4.setData(modelMatrix4);

        modelObject->vertexArray.bind();

        for(const auto& mesh : modelObject->meshes)
        {
            mesh->getIndexBuffer().bind();
            glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndexBuffer().getElementAmount(), GL_UNSIGNED_INT, 0, modelIterator.second.size());
        }

        modelObject->vertexArray.unbind();
    }

    mOrders.clear();
    mShader.deactivate();
}

std::type_index ModelRenderer::getRenderableType() const
{
    return std::type_index(typeid(ModelRenderable));   
}
