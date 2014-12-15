#pragma once
#include <fea/util.hpp>
#include "renderer.hpp"
#include "glcontext.hpp"
#include "renderingmessages.hpp"
#include "../resources/resourcemessages.hpp"
#include "../networking/networkingprotocol.hpp"
#include "../input/inputmessages.hpp"
#include "../world/worldmessages.hpp"
//test
#include "debugrenderable.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "modelrenderable.hpp"
#include "chunkmodelcreator.hpp"

class RenderingSystem :
    public fea::MessageReceiver<AddGfxEntityMessage,
                                RotateGfxEntityMessage,
                                MoveGfxEntityMessage,
                                RemoveGfxEntityMessage,
                                ClientAttachedToEntityMessage,
                                WindowResizeMessage,
                                RenderModeMessage,
                                ModelDeliverMessage,
                                ShaderSourceDeliverMessage,
                                ShaderDefinitionDeliverMessage,
                                TextureDeliverMessage,
                                UpdateChunkVboMessage,
                                ChunkDeletedMessage>
{
    enum RenderModule{ DEBUG, MODEL, VOXEL };
    public:
        RenderingSystem(fea::MessageBus& bus, const glm::uvec2& viewSize);
        void handleMessage(const AddGfxEntityMessage& received) override;
        void handleMessage(const RotateGfxEntityMessage& received) override;
        void handleMessage(const MoveGfxEntityMessage& received) override;
        void handleMessage(const RemoveGfxEntityMessage& received) override;
        void handleMessage(const ClientAttachedToEntityMessage& received) override;
        void handleMessage(const WindowResizeMessage& received) override;
        void handleMessage(const RenderModeMessage& received) override;
        void handleMessage(const ModelDeliverMessage& received) override;
        void handleMessage(const ShaderSourceDeliverMessage& received) override;
        void handleMessage(const ShaderDefinitionDeliverMessage& received) override;
        void handleMessage(const TextureDeliverMessage& received) override;
        void handleMessage(const UpdateChunkVboMessage& received) override;
        void handleMessage(const ChunkDeletedMessage& received) override;
        void render();
    private:
        fea::MessageBus& mBus;
        GLContext mGLContext;
        Renderer mRenderer;
        uint32_t mCameraEntity;

        //test
        std::vector<DebugRenderable> mDebuggers;
        std::vector<std::unique_ptr<Model>> mModels;

        std::unordered_map<int32_t, ModelRenderable> mModelRenderables;
        std::unordered_map<std::string, std::string> mVertexSources;
        std::unordered_map<std::string, std::string> mFragmentSources;
        std::unordered_map<std::string, std::unique_ptr<Shader>> mShaders;

        ChunkModelCreator mChunkModelCreator;
        std::unordered_map<ChunkCoord, Model> mChunkModels;
};
