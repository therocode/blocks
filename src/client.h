#pragma once
#include <SFML/Window.hpp>
#include <featherkit/userinterface.h>
#include "messages.h"
#include "renderer.h"
#include "input/inputadaptor.h"
#include "serverclientbridge.h"

class Client : public fea::MessageReceiver<InputActionMessage>
{
    public:
        Client();
        void setup();
        void handleInput();
        void render();
        void destroy();
        virtual void handleMessage(const InputActionMessage& received);
        bool requestedQuit();
        void setServerBridge(std::unique_ptr<ServerClientBridge> bridge);
    private:
        fea::MessageBus mBus;
        sf::Window sfWindow;
        fea::Window window;
        Renderer renderer;
        InputAdaptor inputAdaptor;
        bool quit;

        std::unique_ptr<ServerClientBridge> mBridge;
};
