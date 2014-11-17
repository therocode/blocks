#pragma once
#include <fea/util.hpp>
#include "renderer.hpp"
#include "glcontext.hpp"
#include "../renderingmessages.hpp"
#include "../../networking/networkingprotocol.hpp"
#include "debugrenderable.hpp"

class RenderingSystem :
    public fea::MessageReceiver<RotateGfxEntityMessage,
                                MoveGfxEntityMessage,
                                ClientAttachedToEntityMessage>
{
    public:
        RenderingSystem(fea::MessageBus& bus);
        void handleMessage(const RotateGfxEntityMessage& received) override;
        void handleMessage(const MoveGfxEntityMessage& received) override;
        void handleMessage(const ClientAttachedToEntityMessage& received) override;
        void render();
    private:
        fea::MessageBus& mBus;
        GLContext mGLContext;
        Renderer mRenderer;
        uint32_t mCameraEntity;

        std::vector<DebugRenderable> mDebuggers;
};
