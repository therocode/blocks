#pragma once
#include "inputactions.h"
#include <featherkit/messaging.h>

struct WindowResize_tag{};//                               width     height
using WindowResizeMessage = fea::Message<WindowResize_tag, uint32_t, uint32_t>;

struct PlayerAction_tag{};//                               playerid  inputaction enum string 
using PlayerActionMessage = fea::Message<PlayerAction_tag, size_t, InputAction>;

struct MouseMoved_tag{};//                             moveX  moveY
using MouseMovedMessage = fea::Message<MouseMoved_tag, float, float>;
