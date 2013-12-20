#pragma once
#include <featherkit/userinterface.h>
#include "../input/inputmessages.h"
#include "../rendering/renderingmessages.h"
#include "../script/scriptmessages.h"
#include "../rendering/renderer.h"
#include "../input/inputadaptor.h"
#include "../networking/serverclientbridge.h"

#include "utilities/lodepng.h"
#include "utilities/logger.h"
#include "application/applicationmessages.h"

#include "utilities/fpscontroller.h"

class Client :
    public fea::MessageReceiver<PlayerActionMessage>,
    public fea::MessageReceiver<PlayerMoveDirectionMessage>,
    public fea::MessageReceiver<PlayerMoveActionMessage>,
    public fea::MessageReceiver<PlayerPitchYawMessage>,
    public fea::MessageReceiver<RebuildScriptsRequestedMessage>,
    public fea::MessageReceiver<WindowFocusLostMessage>,
    public fea::MessageReceiver<WindowInputMessage>
{
    public:
        Client();
        ~Client();
        bool loadTexture(const std::string& path, uint32_t width, uint32_t height, std::vector<unsigned char>& result);
        void setup();
        void handleInput();
        void render();
        void destroy();
        virtual void handleMessage(const PlayerActionMessage& received);
        virtual void handleMessage(const PlayerMoveDirectionMessage& received);
        virtual void handleMessage(const PlayerMoveActionMessage& received);
        virtual void handleMessage(const PlayerPitchYawMessage& received);
        virtual void handleMessage(const RebuildScriptsRequestedMessage& received);
        virtual void handleMessage(const WindowFocusLostMessage& received);
        virtual void handleMessage(const WindowInputMessage& received);
        bool requestedQuit();
        void setServerBridge(std::unique_ptr<ServerClientBridge> bridge);
        fea::MessageBus& getBus();
    private:
        long mFrame = 0;
        FPSController mFPSCounter;
        void fetchServerData();
        void updateChunk(const ChunkCoord& coordinate);
        bool mLockedMouse;
        fea::MessageBus mBus;
        Logger mLogger;
        fea::Window mWindow;
        Renderer mRenderer;
        InputAdaptor mInputAdaptor;
        bool mQuit;
        std::string mLogName;

        ChunkMap mLocalChunks;

        std::unique_ptr<ServerClientBridge> mBridge;
};
