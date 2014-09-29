#pragma once
#include <functional>

class LocalServerClientBridge;

struct LocalConnectionAttemptMessage
{
    mutable LocalServerClientBridge* clientToServerBridge;
};
