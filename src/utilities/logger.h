#pragma once
#include <featherkit/messaging.h>
#include "messages.h"

class Logger : public fea::MessageReceiver<LogMessage>
{
    public:
        Logger(fea::MessageBus& bus);
        ~Logger();
        virtual void handleMessage(const LogMessage& received);
        std::string getTimeString() const;
        void printLine(const std::string& lineStart, const std::string& message) const;
        std::vector<std::string> explode(const std::string& str, const char& ch) const;
    private:
        fea::MessageBus& mBus;
};
