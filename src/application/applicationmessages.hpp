#pragma once
#include <fea/util.hpp>
#include "../blockstd.hpp"
#include "../utilities/loglevel.hpp"
#include "../world/worldid.hpp"

struct FrameMessage
{
    bool beginning;
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
    size_t playerId; WorldId worldId; const::glm::vec3& position;
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
