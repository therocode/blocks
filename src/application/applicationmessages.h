#pragma once
#include <fea/util.hpp>
#include "../blockstd.h"
#include "../utilities/loglevel.h"

FEA_DECLARE_MESSAGE(FrameMessage, bool beginning;);
FEA_DECLARE_MESSAGE(LogMessage, const std::string& message; const std::string& component; uint32_t level;);
FEA_DECLARE_MESSAGE(LogLevelMessage, uint32_t level;);
FEA_DECLARE_MESSAGE(FatalMessage, const std::string& message;);
FEA_DECLARE_MESSAGE(PlayerJoinedMessage, size_t playerId; const::glm::vec3& position;);
FEA_DECLARE_MESSAGE(PlayerDisconnectedMessage, size_t playerId;);
FEA_DECLARE_MESSAGE(GameStartMessage);
FEA_DECLARE_MESSAGE(WindowFocusLostMessage);
FEA_DECLARE_MESSAGE(FocusGainedMessage);
FEA_DECLARE_MESSAGE(WindowInputMessage);
