#pragma once
#include <fea/util.hpp>
#include "../utilities/glm.hpp"
#include "../utilities/loglevel.hpp"
#include "../world/worlddefines.hpp"

struct FrameMessage
{
    uint64_t frameNumber;
    int32_t deltaTime;
};

struct LogMessage
{
    const std::string& message; const std::string& component; uint32_t level;
};

struct LogLevelMessage
{
    uint32_t level;
};

struct PlayerJoinedGameMessage
{
    size_t playerId;
};

struct PlayerLeftGameMessage
{
    size_t playerId;
};

struct GameStartMessage
{
};

struct WindowFocusLostMessage
{
};

struct FocusGainedMessage
{
};

struct WindowInputMessage
{
};
