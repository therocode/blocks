#pragma once
#include "clientconnectionlistener.hpp"

class LocalServerClientBridge;

class LocalClientConnectionListener : public ClientConnectionListener
{
    public:
        void createClientConnection(LocalServerClientBridge* clientBridge);
};
