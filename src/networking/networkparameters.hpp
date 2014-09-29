#pragma once
#include <string>

enum class NetworkMode { SINGLE_PLAYER, DEDICATED, JOIN, COMBINED };

struct NetworkParameters
{
    NetworkMode mode;
    std::string serverName;
    int32_t port;
};
