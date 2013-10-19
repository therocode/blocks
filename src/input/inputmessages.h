#pragma once
#include "inputactions.h"
#include <featherkit/messaging.h>

struct WindowResize_tag{};//                               width                 height
using WindowResizeMessage = fea::Message<WindowResize_tag, const uint32_t, const uint32_t>;

struct InputAction_tag{};//                              inputaction enum string thing 
using InputActionMessage = fea::Message<InputAction_tag, const InputAction>;

struct MouseMoved_tag{};//                                moveX         moveY
using MouseMovedMessage = fea::Message<MouseMoved_tag, const float, const float>;
