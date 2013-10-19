#pragma once
#include <featherkit/messaging.h>

struct Frame_tag{};//                                       true if beginning of frame, false if ending of frame
using FrameMessage = fea::Message<Frame_tag, bool>;

struct Log_tag{};//                      message             component
using LogMessage = fea::Message<Log_tag, const std::string&, const std::string&>;
