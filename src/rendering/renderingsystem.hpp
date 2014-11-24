#pragma once
#include <fea/util.hpp>
#include "renderer.hpp"
#include "glcontext.hpp"
#include "renderingmessages.hpp"
#include "../networking/networkingprotocol.hpp"
#include "../input/inputmessages.hpp"
//test
#include "debugrenderable.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "modelrenderable.hpp"

class RenderingSystem :
    public fea::MessageReceiver<RotateGfxEntityMessage,
                                MoveGfxEntityMessage,
                                ClientAttachedToEntityMessage,
                                WindowResizeMessage,
                                RenderModeMessage>
{
    enum RenderModule{ DEBUG, MODEL };
    public:
        RenderingSystem(fea::MessageBus& bus, const glm::uvec2& viewSize);
        void handleMessage(const RotateGfxEntityMessage& received) override;
        void handleMessage(const MoveGfxEntityMessage& received) override;
        void handleMessage(const ClientAttachedToEntityMessage& received) override;
        void handleMessage(const WindowResizeMessage& received) override;
        void handleMessage(const RenderModeMessage& received) override;
        void render();
    private:
        fea::MessageBus& mBus;
        GLContext mGLContext;
        Renderer mRenderer;
        uint32_t mCameraEntity;

        //test
        std::vector<DebugRenderable> mDebuggers;

        Mesh mMesh;
        Model mModel;
        ModelRenderable mModelRenderable;
};
