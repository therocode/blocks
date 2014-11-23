#pragma once
#include "rendermodule.hpp"
#include "modelrenderable.hpp"
#include "opengl.hpp"
#include "vao.hpp"
#include "buffer.hpp"
#include "shader.hpp"

struct ModelOrder
{
    const Model* model;
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

        std::vector<ModelOrder> mOrders;
};
