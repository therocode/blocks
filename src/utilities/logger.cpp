#include "logger.h"
#include "../application/applicationmessages.h"
#include <chrono>
#include <iostream>
#include "../console/console.h"

Logger::Logger(fea::MessageBus& bus) : mBus(bus),
                                       mLogLevel(LogLevel::INFO)
{
    mBus.addMessageSubscriber<LogMessage>(*this);
}

Logger::~Logger()
{
    mBus.removeMessageSubscriber<LogMessage>(*this);
}

void Logger::handleMessage(const LogMessage& received)
{
    std::string message;
    std::string component;
    LogLevel level;

    std::tie(message, component, level) = received.data;


    for(auto& line : explode(message, '\n'))
    {
        printLine("[" + getTimeString() + "|" + component + "]: ", line);
    }
}

void Logger::handleMessage(const LogLevelMessage& received)
{
    std::tie(mLogLevel) = received.data;

    if(mLogLevel > LogLevel::VERBOSE)
        mLogLevel = LogLevel::VERBOSE;
}

std::string Logger::getTimeString() const
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm *parts = std::localtime(&now_c);

    std::string hour = parts->tm_hour < 10 ? std::string("0") + std::to_string(parts->tm_hour) : std::to_string(parts->tm_hour);
    std::string min = parts->tm_min < 10 ? std::string("0") + std::to_string(parts->tm_min) : std::to_string(parts->tm_min);
    std::string sec = parts->tm_sec < 10 ? std::string("0") + std::to_string(parts->tm_sec) : std::to_string(parts->tm_sec);

    return hour + ":" + min + ":" + sec;
}

void Logger::printLine(const std::string& lineStart, const std::string& message) const
{
    //Console::SetFGColor(ConsoleColour::RED);
    Console::Write(lineStart + message + "\n");
    //std::cout << lineStart << message << "\n";
}

std::vector<std::string> Logger::explode(const std::string& str, const char& ch)  const
{
    std::string next;
    std::vector<std::string> result;

    // For each character in the string
    for (std::string::const_iterator it = str.begin(); it != str.end(); it++) 
    {
        // If we've hit the terminal character
        if (*it == ch) 
        {
            // If we have some characters accumulated
            if (!next.empty()) 
            {
                // Add them to the result vector
                result.push_back(next);
                next.clear();
            }
        } else 
        {
            // Accumulate the next character into the sequence
            next += *it;
        }
    }
    if (!next.empty())
        result.push_back(next);
    return result;
}
