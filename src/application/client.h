#pragma once
#include <featherkit/userinterface.h>
#include "../input/inputmessages.h"
#include "../rendering/renderingmessages.h"
#include "../script/scriptmessages.h"
#include "../rendering/renderer.h"
#include "../input/inputadaptor.h"
#include "../networking/serverclientbridge.h"

class Client : 
    public fea::MessageReceiver<PlayerActionMessage>,
    public fea::MessageReceiver<PlayerMoveDirectionMessage>,
    public fea::MessageReceiver<PlayerMoveActionMessage>,
    public fea::MessageReceiver<PlayerPitchYawMessage>,
    public fea::MessageReceiver<RebuildScriptsRequestedMessage>
{
    public:
        Client();
        ~Client();
        void setup();
        void handleInput();
        void render();
        void destroy();
        virtual void handleMessage(const PlayerActionMessage& received);
        virtual void handleMessage(const PlayerMoveDirectionMessage& received);
        virtual void handleMessage(const PlayerMoveActionMessage& received);
        virtual void handleMessage(const PlayerPitchYawMessage& received);
        virtual void handleMessage(const RebuildScriptsRequestedMessage& received);
        bool requestedQuit();
        void setServerBridge(std::unique_ptr<ServerClientBridge> bridge);
    private:
        void fetchServerData();
        fea::MessageBus mBus;
        fea::Window mWindow;
        Renderer mRenderer;
        InputAdaptor mInputAdaptor;
        bool mQuit;
        std::string mLogName;

        ChunkMap mLocalChunks;

        std::unique_ptr<ServerClientBridge> mBridge;
};
