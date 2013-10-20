#pragma once
#include <featherkit/messaging.h>
#include "../blockstd.h"

struct Frame_tag{};//                                       true if beginning of frame, false if ending of frame
using FrameMessage = fea::Message<Frame_tag, bool>;

struct Log_tag{};//                      message             component
using LogMessage = fea::Message<Log_tag, const std::string&, const std::string&>;

struct PlayerJoined_tag{};//                               player id   position
using PlayerJoinedMessage = fea::Message<PlayerJoined_tag, size_t, const::glm::vec3&>;
