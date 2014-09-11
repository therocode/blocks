#pragma once
#include "inputactions.hpp"
#include <fea/util.hpp>

FEA_DECLARE_MESSAGE(WindowResizeMessage, uint32_t width; uint32_t height;);
FEA_DECLARE_MESSAGE(PlayerActionMessage, size_t playerId; InputAction action;);
FEA_DECLARE_MESSAGE(PlayerPitchYawMessage, size_t playerId; float pitch; float yaw;);
