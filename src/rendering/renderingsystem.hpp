#pragma once
#include <fea/util.hpp>
#include "renderer.hpp"
#include "glcontext.hpp"
#include "renderingmessages.hpp"
#include "../resources/resourcemessages.hpp"
#include "../resources/texturedefinition.hpp"
#include "../resources/gfxentitydefinition.hpp"
#include "../networking/networkingprotocol.hpp"
#include "../input/inputmessages.hpp"
#include "../world/worldmessages.hpp"
#include "../utilities/idpool.hpp"
#include "../utilities/threadpool.hpp"
//test
#include "debugrenderable.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "modelrenderable.hpp"
#include "chunkmodelcreator.hpp"
#include "texturearray.hpp"

class RenderingSystem :
    public fea::MessageReceiver<AddGfxEntityMessage,
                                OrientateGfxEntityMessage,
                                MoveGfxEntityMessage,
                                RemoveGfxEntityMessage,
                                AttachedToGfxEntityMessage,
                                AnimateGfxEntityMessage,
                                WindowResizeMessage,
                                RenderModeMessage,
                                ResourceDeliverMessage<RawModel>,
                                ResourceDeliverMessage<ShaderSource>,
                                ResourceDeliverMessage<ShaderDefinition>,
                                ResourceDeliverMessage<TextureArray>,
                                ResourceDeliverMessage<TextureDefinition>,
                                ResourceDeliverMessage<GfxEntityDefinition>,
                                UpdateChunkVboMessage,
                                ChunkDeletedMessage,
                                FacingBlockMessage>
{
    enum RenderModule{ DEBUG, MODEL, VOXEL, EXTRA };
    public:
        RenderingSystem(fea::MessageBus& bus, const glm::uvec2& viewSize);
        void handleMessage(const AddGfxEntityMessage& received) override;
        void handleMessage(const OrientateGfxEntityMessage& received) override;
        void handleMessage(const MoveGfxEntityMessage& received) override;
        void handleMessage(const RemoveGfxEntityMessage& received) override;
        void handleMessage(const AttachedToGfxEntityMessage& received) override;
        void handleMessage(const AnimateGfxEntityMessage& received) override;
        void handleMessage(const WindowResizeMessage& received) override;
        void handleMessage(const RenderModeMessage& received) override;
        void handleMessage(const ResourceDeliverMessage<RawModel>& received) override;
        void handleMessage(const ResourceDeliverMessage<ShaderSource>& received) override;
        void handleMessage(const ResourceDeliverMessage<ShaderDefinition>& received) override;
        void handleMessage(const ResourceDeliverMessage<TextureArray>& received) override;
        void handleMessage(const ResourceDeliverMessage<TextureDefinition>& received) override;
        void handleMessage(const ResourceDeliverMessage<GfxEntityDefinition>& received) override;
        void handleMessage(const UpdateChunkVboMessage& received) override;
        void handleMessage(const ChunkDeletedMessage& received) override;
        void handleMessage(const FacingBlockMessage& received) override;
        void render();
    private:
        ChunkModelDelivery generateChunkModel(ChunkCoord coordinate, Chunk main, std::shared_ptr<Chunk> top, std::shared_ptr<Chunk> bottom, std::shared_ptr<Chunk> front, std::shared_ptr<Chunk> back, std::shared_ptr<Chunk> left, std::shared_ptr<Chunk> right);
        void fetchDoneChunkMeshes();

        fea::MessageBus& mBus;
        GLContext mGLContext;
        Renderer mRenderer;
        uint32_t mCameraEntity;

        IdPool<size_t> mEntityIds;

        std::unordered_map<uint32_t, std::unique_ptr<Model>> mModels;

        std::unordered_map<int32_t, ModelRenderable> mModelRenderables;
        std::unordered_map<std::string, std::string> mVertexSources;
        std::unordered_map<std::string, std::string> mFragmentSources;
        std::unordered_map<uint32_t, std::unique_ptr<Shader>> mBaseShaders;
        std::unordered_map<uint32_t, std::unique_ptr<Shader>> mAnimationShaders;
        std::unordered_map<uint32_t, std::shared_ptr<TextureArray>> mTextureArrays;
        std::unordered_map<uint32_t, std::shared_ptr<TextureDefinition>> mTextureDefinitions;
        std::unordered_map<uint32_t, std::shared_ptr<GfxEntityDefinition>> mGfxEntityDefinitions;

        std::unordered_map<ChunkCoord, std::pair<bool, ChunkModel>> mChunkModels;

        bool mIsFacing;
        glm::ivec3 mFacingBlock;

        ThreadPool mMeshWorkerPool;
        std::unordered_set<ChunkCoord> mActiveChunks;
        std::vector<std::future<ChunkModelDelivery>> mChunkModelsToFinish;
};
