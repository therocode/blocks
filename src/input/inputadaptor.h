#pragma once
#include <featherkit/userinterface.h>
#include <featherkit/messaging.h>
#include "../entity/entitymessages.h"

class InputAdaptor : public fea::MessageReceiver<PlayerIdMessage>
{
    public:
        InputAdaptor(fea::MessageBus& b);
        ~InputAdaptor();
        void update();
        void handleMessage(const PlayerIdMessage& received) override;

    private:
        fea::MessageBus& mBus;
        fea::InputHandler inputHandler;
        fea::ActionHandler<std::string> actionHandler;

        size_t mPlayerId;

		bool windowFocus;
        int32_t lastMouseX;
        int32_t lastMouseY;
        bool first;

        bool mouseDown;
};
