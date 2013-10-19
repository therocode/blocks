#pragma once
#include <featherkit/messaging.h>
#include "messages.h"

class Logger : public fea::MessageReceiver<LogMessage>
{
    public:
        Logger(fea::MessageBus& bus);
        ~Logger();
        void handleMessage(const LogMessage& received);
    private:
        fea::MessageBus& mBus;
};
