#pragma once
#include <featherkit/messaging.h>

struct RebuildScriptsRequested_tag{};//                                          nothing
using RebuildScriptsRequestedMessage = fea::Message<RebuildScriptsRequested_tag, char>;
