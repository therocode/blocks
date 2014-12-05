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

struct ModelObject
{
    VAO vertexArray;
    std::vector<const Mesh*> meshes;
    Buffer colors;
    Buffer modelMatrix1;
    Buffer modelMatrix2;
    Buffer modelMatrix3;
    Buffer modelMatrix4;
};

class ModelRenderer : public RenderModule
{
    public:
        ModelRenderer();
        void queue(const Renderable& renderable) override;
        void render(const Camera& camera, const glm::mat4& perspective, const Shader& shader) override;
        std::type_index getRenderableType() const override;
    private:
        VAO mVertexArray;

        std::unordered_map<const Model*, std::vector<ModelOrder>> mOrders;

        //mesh vao cache
        std::unordered_map<const Model*, std::unique_ptr<ModelObject>> mModelCache;
};
