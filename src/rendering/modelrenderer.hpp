#pragma once
#include "rendermodule.hpp"
#include "modelrenderable.hpp"
#include "modelattribute.hpp"
#include "opengl.hpp"
#include "vao.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "modeltexturearrayhash.hpp"

class Mesh;
class Animation;

struct ModelOrder
{
    const Model* model;
    const TextureArray* textureArray;
    uint32_t textureIndex;
    glm::vec3 color;
    glm::vec3 position;
	glm::quat orientation;
    float frameOffset;
    std::string animation;
};

struct ModelBufferStorage
{
    ModelBufferStorage();
    VAO vertexArray;

    //model data
    std::unordered_map<ModelAttribute, Buffer> modelBuffers;
    std::vector<Buffer> meshes;

    //per-instance data
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

struct ModelInstance
{
    const Animation* animation;
};

class ModelRenderer : public RenderModule
{
    public:
        ModelRenderer();
        void queue(const Renderable& renderable) override;
        void render(const Camera& camera, const glm::mat4& perspective, const Shader& shader) override;
        std::type_index getRenderableType() const override;
    private:
        void cacheModel(const Model& model);
        void uploadBatchData(std::vector<ModelOrder>::const_iterator startOrder, int32_t orderAmount, const Camera& camera, const Shader& shader, const Model& model, ModelBufferStorage& modelBufferStorage);
        VAO mVertexArray;

        std::unordered_map<std::pair<const Model*, const TextureArray*>, std::vector<ModelOrder>> mOrders;

        GLint mAnimationBlockLocation;
        //model cache
        std::unordered_map<const Model*, std::unique_ptr<ModelBufferStorage>> mModelBufferCache;

        //cached arrays
        std::vector<float> mColors;
        std::vector<float> mTotalAnimData;
        std::vector<float> mTotalRotData;
        std::vector<float> mTotalTransData;
        //... more can be optimised like this
};
