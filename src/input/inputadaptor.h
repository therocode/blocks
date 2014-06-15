#pragma once
#include <fea/userinterface.hpp>
#include <fea/messaging.hpp>
#include "../entity/entitymessages.h"
#include "../entity/controllers/movedirection.h"
#include "application/applicationmessages.h"

class InputAdaptor : public fea::MessageReceiver<PlayerIdMessage>
{
    public:
        InputAdaptor(fea::MessageBus& b);
        ~InputAdaptor();
        void update();
        void handleMessage(const PlayerIdMessage& received) override;

    private:
        void sendMovementData();
        fea::MessageBus& mBus;
        fea::InputHandler inputHandler;
        fea::ActionHandler<std::string> actionHandler;

        size_t mPlayerId;

		bool windowFocus;
        int32_t lastMouseX;
        int32_t lastMouseY;
        bool first;

        bool mouseDown;

        bool mHoldingForwards;
        bool mHoldingBackwards;
        bool mHoldingLeft;
        bool mHoldingRight;

        float mNewPitch;
        float mNewYaw;
        
        MoveDirection direction;
};
