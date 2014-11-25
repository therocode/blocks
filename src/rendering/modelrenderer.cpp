#include "modelrenderer.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "mesh.hpp"
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
    FEA_ASSERT(order.model->findMesh(0) != nullptr, "Trying to render a model renderable which has a model without a primary mesh");

    mOrders[order.model->findMesh(0)].push_back(order);
}

void ModelRenderer::render(const Camera& camera, const glm::mat4& perspective)
{
    mVertexArray.bind();
    mShader.activate();

    mShader.setUniform("viewProjectionMatrix", UniformType::MAT4X4, glm::value_ptr(perspective * camera.getMatrix()));
    
    for(const auto meshIterator : mOrders)
    {
        const Mesh& mesh = *meshIterator.first;

        const auto iterator = mMeshCache.find(&mesh);
        if(iterator == mMeshCache.end())
        {
            std::unique_ptr<MeshObject> newMeshObject = std::unique_ptr<MeshObject>(new MeshObject());

            newMeshObject->vertexArray.setVertexAttribute(ModelAttribute::POSITION, 3, mesh.getPositionBuffer());
            newMeshObject->vertexArray.setInstanceAttribute(ModelAttribute::COLOR, 3, newMeshObject->colors, 1);
            newMeshObject->vertexArray.setInstanceAttribute(ModelAttribute::MODELMATRIX1, 4, newMeshObject->modelMatrix1, 1);
            newMeshObject->vertexArray.setInstanceAttribute(ModelAttribute::MODELMATRIX2, 4, newMeshObject->modelMatrix2, 1);
            newMeshObject->vertexArray.setInstanceAttribute(ModelAttribute::MODELMATRIX3, 4, newMeshObject->modelMatrix3, 1);
            newMeshObject->vertexArray.setInstanceAttribute(ModelAttribute::MODELMATRIX4, 4, newMeshObject->modelMatrix4, 1);

            mMeshCache.emplace(&mesh, std::move(newMeshObject));
        }


        const auto& meshObject = mMeshCache.at(&mesh);

        std::vector<float> colors;
        std::vector<float> modelMatrix1;
        std::vector<float> modelMatrix2;
        std::vector<float> modelMatrix3;
        std::vector<float> modelMatrix4;

        std::vector<float> data1 = { 1.0f, 0.0f, 0.0f, 0.0f };
        std::vector<float> data2 = { 0.0f, 1.0f, 0.0f, 0.0f };
        std::vector<float> data3 = { 0.0f, 0.0f, 1.0f, 0.0f };
        std::vector<float> data4 = { 0.0f, 0.0f, 0.0f, 1.0f };
        
        for(const auto order : meshIterator.second)
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

        meshObject->colors.setData(colors);
        meshObject->modelMatrix1.setData(modelMatrix1);
        meshObject->modelMatrix2.setData(modelMatrix2);
        meshObject->modelMatrix3.setData(modelMatrix3);
        meshObject->modelMatrix4.setData(modelMatrix4);

        meshObject->vertexArray.bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.getPositionBuffer().getElementAmount() / 3, meshIterator.second.size());
        meshObject->vertexArray.unbind();
    }

    mOrders.clear();
    mShader.deactivate();
}

std::type_index ModelRenderer::getRenderableType() const
{
    return std::type_index(typeid(ModelRenderable));   
}
