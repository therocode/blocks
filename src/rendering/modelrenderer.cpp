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
//layout(location = ~COLOR~) in vec3 color;
//layout(location = ~MODELMATRIX1~) in vec4 modelMatrix1;
//layout(location = ~MODELMATRIX2~) in vec4 modelMatrix2;
//layout(location = ~MODELMATRIX3~) in vec4 modelMatrix3;
//layout(location = ~MODELMATRIX4~) in vec4 modelMatrix4;

out vec3 objectColor;

void main()
{
    //mat4 modelMatrix = mat4( modelMatrix1, modelMatrix2, modelMatrix3, modelMatrix4 );
    mat4 modelMatrix = mat4(1);
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(vec3(in_position.x, in_position.y, in_position.z), 1.0);
    //objectColor = color;
    objectColor = vec3(1.0f, 1.0f, 1.0f);
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
    
    FEA_ASSERT(order.model != nullptr, "Trying to render a model renderable which doesn't have a model");
    FEA_ASSERT(order.model->findMesh(0) != nullptr, "Trying to render a model renderable which has a model without a primary mesh");

    mOrders.push_back(order);
}

void ModelRenderer::render(const Camera& camera, const glm::mat4& perspective)
{
    mVertexArray.bind();
    mShader.activate();

    mShader.setUniform("viewProjectionMatrix", UniformType::MAT4X4, glm::value_ptr(perspective * camera.getMatrix()));
    
    for(const auto order : mOrders)
    {
        const Mesh& mesh = *order.model->findMesh(0);

        const auto iterator = mMeshCache.find(&mesh);

        if(iterator == mMeshCache.end())
        {
            //create gpu side mesh
            std::unique_ptr<MeshObject> newMeshObject = std::unique_ptr<MeshObject>(new MeshObject());

            newMeshObject->vertexArray.setVertexAttribute(ModelAttribute::POSITION, 3, mesh.getPositionBuffer());

            //render it
            newMeshObject->vertexArray.bind();
            glDrawArrays(GL_TRIANGLES, 0, mesh.getPositionBuffer().getElementAmount() / 3);
            newMeshObject->vertexArray.unbind();

            //store it
            mMeshCache.emplace(&mesh, std::move(newMeshObject));
        }
        else
        {
            auto& meshObject = iterator->second;
            
            meshObject->vertexArray.bind();
            glDrawArrays(GL_TRIANGLES, 0, mesh.getPositionBuffer().getElementAmount() / 3);
            meshObject->vertexArray.unbind();
        }
    }

    mOrders.clear();
    mShader.deactivate();
}

std::type_index ModelRenderer::getRenderableType() const
{
    return std::type_index(typeid(ModelRenderable));   
}
