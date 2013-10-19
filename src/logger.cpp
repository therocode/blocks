#include "logger.h"
#include "messages.h"
#include <chrono>
#include <iostream>

Logger::Logger(fea::MessageBus& bus) : mBus(bus)
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

    std::tie(message, component) = received.data;

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm *parts = std::localtime(&now_c);

    std::string hour = parts->tm_hour < 10 ? std::string("0") + std::to_string(parts->tm_hour) : std::to_string(parts->tm_hour);
    std::string min = parts->tm_min < 10 ? std::string("0") + std::to_string(parts->tm_min) : std::to_string(parts->tm_min);
    std::string sec = parts->tm_sec < 10 ? std::string("0") + std::to_string(parts->tm_sec) : std::to_string(parts->tm_sec);

    std::cout << "[" << hour << ":" << min << ":" << sec << "|" << component << "]: " << message;
}
