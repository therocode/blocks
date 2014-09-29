#pragma once
#include "inputactions.hpp"
#include <fea/util.hpp>

struct WindowResizeMessage
{
    uint32_t width; uint32_t height;
};

struct PlayerActionMessage
{
    size_t playerId; InputAction action;
};

struct PlayerPitchYawMessage
{
    size_t playerId; float pitch; float yaw;
};

struct CursorLockedMessage
{
    bool locked;
};
