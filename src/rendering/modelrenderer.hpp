#pragma once
#include "rendermodule.hpp"
#include "modelrenderable.hpp"
#include "opengl.hpp"
#include "vao.hpp"
#include "buffer.hpp"
#include "shader.hpp"

class Mesh;
class Animation;

struct ModelOrder
{
    const Model* model;
    const TextureArray* textureArray;
    uint32_t textureIndex;
    glm::vec3 color;
    glm::vec3 position;
    float pitch;
    float yaw;
};

struct ModelObject
{
    VAO vertexArray;
    std::vector<const Mesh*> meshes;
    const Animation* animation;
    Buffer colors;
    Buffer textureIndices;
    Buffer modelMatrix1;
    Buffer modelMatrix2;
    Buffer modelMatrix3;
    Buffer modelMatrix4;
    Buffer normalMatrix1;
    Buffer normalMatrix2;
    Buffer normalMatrix3;
    Buffer normalMatrix4;
    Buffer animData;
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

        float mCurFrame;
        bool bajs = false;
};
