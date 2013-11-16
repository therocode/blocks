#pragma once
#include "clientconnectionlistener.h"
#include "localserverclientbridge.h"

class LocalClientConnectionListener : public ClientConnectionListener
{
    public:
        void createClientConnection(LocalServerClientBridge* clientBridge);
};
