#pragma once
#include <fea/util.hpp>
#include "../utilities/glm.hpp"
#include "../utilities/loglevel.hpp"
#include "../world/worldconstants.hpp"

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

struct PlayerJoinedMessage
{
    size_t playerId; WorldId worldId; const::glm::vec3& position;
};

struct PlayerDisconnectedMessage
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
