#pragma once
#include "inputactions.h"
#include <featherkit/messaging.h>

struct WindowResize_tag{};//                               width     height
using WindowResizeMessage = fea::Message<WindowResize_tag, uint32_t, uint32_t>;

struct PlayerAction_tag{};//                               playerid  inputaction enum string 
using PlayerActionMessage = fea::Message<PlayerAction_tag, size_t, InputAction>;

struct PlayerPitchYaw_tag{};//                                 player  pitch  yaw
using PlayerPitchYawMessage = fea::Message<PlayerPitchYaw_tag, size_t, float, float>;
