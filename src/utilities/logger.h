#pragma once
#include "../application/applicationmessages.h"
#include "loglevel.h"

class Logger : public fea::MessageReceiver<LogMessage>,
               public fea::MessageReceiver<LogLevelMessage>
{
    public:
        Logger(fea::MessageBus& bus);
        ~Logger();
        virtual void handleMessage(const LogMessage& received);
        virtual void handleMessage(const LogLevelMessage& received);
        std::string getTimeString() const;
        void printLine(const std::string& lineStart, const std::string& message) const;
        std::vector<std::string> explode(const std::string& str, const char& ch) const;
    private:
        fea::MessageBus& mBus;
        LogLevel mLogLevel;
};
