#pragma once
#include <queue>
#include <memory>
#include "clientconnection.h"

class ClientConnectionListener
{
    public:
        std::shared_ptr<ClientConnection> fetchIncomingConnection();
    protected:
        std::queue<std::shared_ptr<ClientConnection> > mIncomingConnections;
};
