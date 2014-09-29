#pragma once
#include "listener.hpp"

class LocalServerClientBridge;

class LocalClientListener : public Listener
{
    public:
        void createClientConnection(LocalServerClientBridge* clientBridge);
};
