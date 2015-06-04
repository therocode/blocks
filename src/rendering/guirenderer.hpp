#pragma once
#include <fea/util.hpp>
#include "rendermodule.hpp"
#include "vao.hpp"
#include "../utilities/gimgui.hpp"
#include "guirenderable.hpp"

class GuiRenderer : public RenderModule
{
    public:
        GuiRenderer(fea::MessageBus& bus);
        void queue(const Renderable& renderable) override;
        void render(const Camera& camera, const glm::mat4& perspective, const Shader& shader) override;
        std::type_index getRenderableType() const override;
        virtual void textureArrayAdded(uint32_t id, TextureArray& textureArray) override;
        virtual void textureDefinitionAdded(const std::string& name, const TextureDefinition& textureDefinition) override;
    private:
        fea::MessageBus& mBus;
        gim::RenderDataGenerator<IVec2Adaptor, RectangleAdaptor, ColorAdaptor> mGenerator;
        std::unordered_map<uint32_t, TextureArray&> mTextureArrays;
        std::unordered_map<uint32_t, TextureDefinition> mTextureDefinitions;
    //std::ifstream fontStream("resources/fonts/LiberationSans-Regular.ttf", std::ios::binary);
    //gim::Font font(fontStream);

        //VAO mVertexArray;
        //Buffer mVertexBuffer;
        //Buffer mModelMatrixBuffer1;
        //Buffer mModelMatrixBuffer2;
        //Buffer mModelMatrixBuffer3;
        //Buffer mModelMatrixBuffer4;
        //Buffer mColorBuffer;

		//VAO mLineVertexArray;
		//Buffer mLineVertexBuffer;
        //Buffer mLineColorBuffer;

        ////frame data
        //std::vector<float> mModelMatrixData1;
        //std::vector<float> mModelMatrixData2;
        //std::vector<float> mModelMatrixData3;
        //std::vector<float> mModelMatrixData4;

        //std::vector<float> mColorData;

        //uint32_t mRenderAmount;

		//std::vector<float> mLineCoords;
		//std::vector<float> mLineColorData;

        //std::vector<float> data1;
        //std::vector<float> data2;
        //std::vector<float> data3;
        //std::vector<float> data4;
};
