#pragma once
#include <featherkit/userinterface.h>
#include <featherkit/messaging.h>
#include "../entity/entitymessages.h"
#include <SFML/Window.hpp>

class InputAdaptor : public fea::MessageReceiver<PlayerIdMessage>
{
    public:
        InputAdaptor(sf::Window& sfw, fea::MessageBus& b);
        ~InputAdaptor();
        void update();
        void handleMessage(const PlayerIdMessage& received) override;

    private:
        sf::Window& sfWindow;
        fea::MessageBus& mBus;
        fea::InputHandler inputHandler;
        fea::ActionHandler<std::string> actionHandler;

        size_t mPlayerId;

        int32_t lastMouseX;
        int32_t lastMouseY;
        bool first;

        bool mouseDown;
};
