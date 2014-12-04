#pragma once
#include <fea/util.hpp>
#include "renderer.hpp"
#include "glcontext.hpp"
#include "renderingmessages.hpp"
#include "../resources/resourcemessages.hpp"
#include "../networking/networkingprotocol.hpp"
#include "../input/inputmessages.hpp"
//test
#include "debugrenderable.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "modelrenderable.hpp"

class RenderingSystem :
    public fea::MessageReceiver<AddGfxEntityMessage,
                                RotateGfxEntityMessage,
                                MoveGfxEntityMessage,
                                RemoveGfxEntityMessage,
                                ClientAttachedToEntityMessage,
                                WindowResizeMessage,
                                RenderModeMessage,
                                ModelDeliverMessage>
{
    enum RenderModule{ DEBUG, MODEL };
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
        void render();
    private:
        fea::MessageBus& mBus;
        GLContext mGLContext;
        Renderer mRenderer;
        uint32_t mCameraEntity;

        //test
        std::vector<DebugRenderable> mDebuggers;

        std::unique_ptr<Mesh> mTetraMesh;
        Model mCubeModel;
        Model mTetraModel;

        std::unordered_map<int32_t, ModelRenderable> mModels;
};
