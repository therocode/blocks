#include "logger.hpp"
#include "../application/applicationmessages.hpp"
#include <chrono>
#include <iostream>
#include "../console/console.hpp"

Logger::Logger(fea::MessageBus& bus, uint32_t logLevel) :
    mBus(bus),
    mLogLevel(logLevel),
    mLongestCategory(0)
{
    Console::Initialise();
    subscribe(mBus, *this);
}

Logger::~Logger()
{
    Console::ResetColor();
}

void Logger::handleMessage(const LogMessage& received)
{
    std::string message = received.message;
    std::string component = received.component;
    uint32_t level = received.level;

    mLongestCategory = std::max((int32_t)component.size(), mLongestCategory);

    if(level <= mLogLevel)
    {
        for(auto& line : explode(message, '\n'))
        {
            printLine("[" + getTimeString() + "|" + component + spaces(mLongestCategory - component.size()) + "|", line, level);
        }
    }
}

void Logger::handleMessage(const LogLevelMessage& received)
{
    mLogLevel = received.level;

    if(mLogLevel > LogLevel::VERB)
        mLogLevel = LogLevel::VERB;
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

void Logger::printLine(const std::string& lineStart, const std::string& message, uint32_t level) const
{
    Console::Write(lineStart);

    switch(level)
    {
        case LogLevel::ERR:
            Console::SetFGColor(ConsoleColour::RED);
            Console::Write(std::string("ERROR  "));
            Console::ResetColor();
            break;
        case LogLevel::WARN:
            Console::SetFGColor(ConsoleColour::YELLOW);
            Console::Write(std::string("WARNING"));
            Console::ResetColor();
            break;
        case LogLevel::INFO:
            Console::Write(std::string("INFO   "));
            break;
        case LogLevel::VERB:
            Console::SetFGColor(ConsoleColour::CYAN);
            Console::Write(std::string("VERBOSE"));
            Console::ResetColor();
            break;
    }

    Console::Write(std::string("]: "));

    Console::Write(message + "\n");
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

std::string Logger::spaces(int32_t amount)
{
    std::string result;

    for(int32_t i = 0; i < amount; i++)
        result.push_back(' ');

    return result;
}
