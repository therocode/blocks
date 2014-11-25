#pragma once
#include "rendermodule.hpp"
#include "modelrenderable.hpp"
#include "opengl.hpp"
#include "vao.hpp"
#include "buffer.hpp"
#include "shader.hpp"

class Mesh;

struct ModelOrder
{
    const Model* model;
    glm::vec3 color;
    glm::vec3 position;
};

struct MeshObject
{
    VAO vertexArray;
    Buffer colors;
    Buffer modelMatrix1;
    Buffer modelMatrix2;
    Buffer modelMatrix3;
    Buffer modelMatrix4;
};

class ModelRenderer : public RenderModule
{
    enum ModelAttribute { POSITION = 0, COLOR, MODELMATRIX1, MODELMATRIX2, MODELMATRIX3, MODELMATRIX4};

    public:
        ModelRenderer();
        void queue(const Renderable& renderable) override;
        void render(const Camera& camera, const glm::mat4& perspective) override;
        std::type_index getRenderableType() const override;
    private:
        VAO mVertexArray;
        Shader mShader;

        std::unordered_map<const Mesh*, std::vector<ModelOrder>> mOrders;

        //mesh vao cache
        std::unordered_map<const Mesh*, std::unique_ptr<MeshObject>> mMeshCache;
};
