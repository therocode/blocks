#pragma once
#include "../application/applicationmessages.hpp"
#include "loglevel.hpp"

class Logger : public fea::MessageReceiver<LogMessage,
                                           LogLevelMessage>
{
    public:
        Logger(fea::MessageBus& bus, uint32_t = LogLevel::INFO);
        ~Logger();
        virtual void handleMessage(const LogMessage& received);
        virtual void handleMessage(const LogLevelMessage& received);
        std::string getTimeString() const;
        void printLine(const std::string& lineStart, const std::string& message, uint32_t level) const;
        std::vector<std::string> explode(const std::string& str, const char& ch) const;
    private:
        std::string spaces(int32_t amount);
        fea::MessageBus& mBus;
        uint32_t mLogLevel;
        int32_t mLongestCategory;
};
