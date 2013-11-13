#pragma once
#include <featherkit/messaging.h>
#include "../blockstd.h"
#include "../utilities/loglevel.h"

struct Frame_tag{};//                                       true if beginning of frame, false if ending of frame
using FrameMessage = fea::Message<Frame_tag, bool>;

struct Log_tag{};//                      message             component
using LogMessage = fea::Message<Log_tag, const std::string&, const std::string&, uint32_t>;

struct LogLevel_tag{};//                           level
using LogLevelMessage = fea::Message<LogLevel_tag, uint32_t>;

struct Fatal_tag{};//                        message
using FatalMessage = fea::Message<Fatal_tag, const std::string&>;

struct PlayerJoined_tag{};//                               player id   position
using PlayerJoinedMessage = fea::Message<PlayerJoined_tag, size_t, const::glm::vec3&>;

struct GameStart_tag{};//
using GameStartMessage = fea::Message<GameStart_tag>;

//Focus lost, yep.
struct FocusLost_tag{};
using WindowFocusLostMessage = fea::Message<FocusLost_tag>;

//probably focus gained. I'm not sure so I made a comment lol. need that 40% comment ratio.
struct FocusGained_tag{};
using FocusGainedMessage = fea::Message<FocusGained_tag>;

//once input is gotten, mouse will be locked maybe.
struct WindowInput_tag{};
using WindowInputMessage = fea::Message<WindowInput_tag>;
