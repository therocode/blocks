#pragma once
#include "clientconnectionlistener.hpp"
#include "localserverclientbridge.hpp"

class LocalClientConnectionListener : public ClientConnectionListener
{
    public:
        void createClientConnection(LocalServerClientBridge* clientBridge);
};
