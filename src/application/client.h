#pragma once
#include <SFML/Window.hpp>
#include <featherkit/userinterface.h>
#include "../input/inputmessages.h"
#include "../rendering/renderingmessages.h"
#include "../script/scriptmessages.h"
#include "../rendering/renderer.h"
#include "../input/inputadaptor.h"
#include "../networking/serverclientbridge.h"

class Client : 
    public fea::MessageReceiver<PlayerActionMessage>,
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
        virtual void handleMessage(const PlayerPitchYawMessage& received);
        virtual void handleMessage(const RebuildScriptsRequestedMessage& received);
        bool requestedQuit();
        void setServerBridge(std::unique_ptr<ServerClientBridge> bridge);
    private:
        void fetchServerData();
        fea::MessageBus mBus;
        sf::Window mSfWindow;
        fea::Window mWindow;
        Renderer mRenderer;
        InputAdaptor mInputAdaptor;
        bool mQuit;
        std::string mLogName;

        std::unique_ptr<ServerClientBridge> mBridge;
};
