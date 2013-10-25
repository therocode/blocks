#pragma once
#include <featherkit/messaging.h>
#include "../blockstd.h"
#include "../utilities/loglevel.h"

struct Frame_tag{};//                                       true if beginning of frame, false if ending of frame
using FrameMessage = fea::Message<Frame_tag, bool>;

struct Log_tag{};//                      message             component
using LogMessage = fea::Message<Log_tag, const std::string&, const std::string&, LogLevel>;

struct LogLevel_tag{};//                           level
using LogLevelMessage = fea::Message<LogLevel_tag, LogLevel>;

struct PlayerJoined_tag{};//                               player id   position
using PlayerJoinedMessage = fea::Message<PlayerJoined_tag, size_t, const::glm::vec3&>;

struct GameStart_tag{};//
using GameStartMessage = fea::Message<GameStart_tag>;
